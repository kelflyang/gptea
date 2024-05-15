import React, { useEffect, useState, useRef } from "react";
import axios from "axios";

const API_KEY = process.env.NEXT_PUBLIC_OPENAI;

export default function Chat({ personIdFrom, personIdTo }) {
  const [pipeline, setPipeline] = useState(true);
  const [personFromMemories, setPersonFromMemories] = useState(null);
  const [personToMemories, setPersonToMemories] = useState(null);
  const mediaRecorder = useRef(null);
  const [stream, setStream] = useState(null);
  const [audioChunks, setAudioChunks] = useState([]);
  const [audio, setAudio] = useState(null);
  const [audioFile, setAudioFile] = useState(null);
  const [transcription, setTranscription] = useState("");
  const [audioBlob, setAudioBlob] = useState("");

  const [messages, setMessages] = useState([]);
  const [messages2, setMessages2] = useState([]);

  const [info, setInfo] = useState({});

  const fetchMemories = async (personId) => {
    try {
      const response = await axios.post("/api/memory_bank/get", { personId });
      console.log(response.data.memories);
      if (personId === personIdFrom) {
        return response.data.memories;
      } else {
        return response.data.memories;
      }
    } catch (error) {
      console.error("Error fetching data:", error);
    }
  };

  const updatePrompt = async () => {
    const fromMemories = await fetchMemories(personIdFrom);
    const toMemories = await fetchMemories(personIdTo);

    setPersonFromMemories(fromMemories);
    setPersonToMemories(toMemories);

    const pipeline1Prompt = `Memories of user:
          ${
            fromMemories
              ? "[" +
                fromMemories
                  .map((m) => {
                    return `"${m.memory}"`;
                  })
                  .join(", ") +
                "]"
              : "[]"
          }

          \n
          
          Memories of the user's grandpa shared with you:
          ${
            toMemories
              ? "[" +
                toMemories
                  .map((m) => {
                    return `"${m.memory}"`;
                  })
                  .join(", ") +
                "]"
              : "[]"
          }
          
          The user does not know about their grandpa's memories. 

          Find a memory from the user's grandpa that the user can relate to and tell it as a story about their grandpa to the user. Keep the story conversational and casual. Use "um"s. Don't make up details you don't have but you can embellish it. 
          `;

    const pipeline2Prompt = `Ask the user questions as if they are a friend you are meeting for coffee.  Keep the tone conversational and casual.`;
    setMessages([{ role: "system", content: pipeline1Prompt }]);
    setMessages2([{ role: "system", content: pipeline2Prompt }]);
  };

  useEffect(async () => {
    updatePrompt();
  }, []);

  useEffect(() => {
    const initializeMediaRecorder = async () => {
      if ("MediaRecorder" in window) {
        try {
          const streamData = await navigator.mediaDevices.getUserMedia({
            audio: true,
          });
          setStream(streamData);
        } catch (err) {
          console.log(err.message);
        }
      } else {
        console.log("The MediaRecorder API is not supported in your browser.");
      }
    };

    initializeMediaRecorder();
  }, []);

  const handleStartRecording = () => {
    const media = new MediaRecorder(stream, { type: "audio/mp3" });

    mediaRecorder.current = media;
    mediaRecorder.current.start();

    let chunks = [];
    mediaRecorder.current.ondataavailable = (e) => {
      chunks.push(e.data);
    };
    setAudioChunks(chunks);
  };
  const handleStopRecording = () => {
    mediaRecorder.current.stop();
    mediaRecorder.current.onstop = () => {
      const audioBlob = new Blob(audioChunks, { type: "audio/mp3" });
      const audioUrl = URL.createObjectURL(audioBlob);

      setAudioBlob(audioBlob);
      setAudio(audioUrl);
      setAudioChunks([]);

      let file = new File([audioUrl], "recorded_audio.mp3", {
        type: "audio/mp3",
        lastModified: new Date().getTime(),
      });
      let container = new DataTransfer();
      container.items.add(file);
      document.getElementById("audioFile").files = container.files;
      setAudioFile(container.files[0]);
    };
  };

  const handleSubmitRecording = async (pipeline) => {
    try {
      const reader = new FileReader();
      reader.onloadend = async () => {
        const base64String = reader.result.split(",")[1]; // Extract base64 data from the result
        // console.log("base64 ", base64String);
        const res = await fetch(`/api/openai/transcribe_audio`, {
          method: "POST",
          headers: {
            "Content-Type": "application/json",
          },
          body: JSON.stringify({ audioBuffer: base64String, lang: "en" }),
        });
        const data = await res.json();
        setTranscription(data.transcription);

        let m;
        if (pipeline) {
          m = [...messages, { role: "user", content: data.transcription }];

          // setMessages(m);
        } else {
          m = [...messages2, { role: "user", content: data.transcription }];
          // setMessages2(m);
        }

        // get chat completion
        const response_ = await fetch(`/api/openai/chat`, {
          method: "POST",
          headers: {
            "Content-Type": "application/json",
          },
          body: JSON.stringify({
            messages: m,
            model: "gpt-4",
          }),
        });

        const data_ = await response_.json();

        if (pipeline === true) {
          setMessages([
            ...messages,
            { role: "user", content: data.transcription },
            data_.completion,
          ]);
        } else {
          setMessages2([
            ...messages2,
            { role: "user", content: data.transcription },
            data_.completion,
          ]);
        }

        const audio = await generateSpeech(data_.completion.content);

        if (pipeline === true) {
          console.log(
            "data completeion for generating speech ",
            data_.completion
          );
          const image = await generateImage(data_.completion.content);
        }
      };
      reader.readAsDataURL(audioBlob);
    } catch (error) {
      console.log(error);
    } finally {
    }
  };

  const generateImage = async (story) => {
    const response = await fetch(`/api/openai/chat`, {
      method: "POST",
      headers: {
        "Content-Type": "application/json",
      },
      body: JSON.stringify({
        messages: [
          {
            role: "system",
            content: `Here is a story being told: ${story}. Summarize it concisely in terms of subject (who is in the story, not including the speaker), setting (where they are) and action (what they are doing). Remove any abstract details.`,
          },
        ],
        model: "gpt-4",
      }),
    });

    const res = await response.json();
    const summary = res.completion.content;
    console.log("summary ", summary);

    const response_ = await fetch(`/api/openai/chat`, {
      method: "POST",
      headers: {
        "Content-Type": "application/json",
      },
      body: JSON.stringify({
        messages: [
          {
            role: "system",
            content: `Here is the breakdown of a story: ${summary}. Generate a concise image prompt. Include 'black and white image' in front of the generated prompt.`,
          },
        ],
        model: "gpt-4",
      }),
    });

    const res_ = await response_.json();
    const prompt = res_.completion.content;
    console.log("prompt ", prompt);

    try {
      const response = await axios.post(
        "https://api.openai.com/v1/images/generations",
        {
          prompt: prompt,
          size: "256x256",
          response_format: "b64_json",
        },
        {
          headers: {
            Authorization: `Bearer ${API_KEY}`,
            "Content-Type": "application/json",
          },
        }
      );

      const res_ = await axios.post("/api/upload_image", {
        url: response.data.data[0].url,
        buffer: response.data.data[0].b64_json,
      });
      return;
    } catch (error) {
      console.error("Error calling API:", error);
      throw error;
    }
  };

  function playAudio(audioData) {
    const context = new AudioContext();
    const audioBuffer =
      audioData instanceof ArrayBuffer
        ? audioData
        : new Uint8Array(audioData).buffer;

    context.decodeAudioData(audioBuffer, function (buffer) {
      const source = context.createBufferSource();
      source.buffer = buffer;
      source.connect(context.destination);
      source.start(0);
    });
  }

  const generateSpeech = async (story) => {
    try {
      const response = await axios.post(
        "https://api.openai.com/v1/audio/speech",
        {
          model: "tts-1",
          input: story,
          voice: "nova",
        },
        {
          headers: {
            Authorization: `Bearer ${API_KEY}`,
            "Content-Type": "application/json",
          },
          responseType: "arraybuffer",
        }
      );

      playAudio(response.data);
    } catch (error) {
      console.error("Error calling API:", error);
      throw error;
    }
  };

  const handleSwitchPipeline = async (pipeline) => {
    if (pipeline) {
      updatePrompt();
    } else {
      console.log("in pupeline 1");

      // get chat completion
      const response_ = await fetch(`/api/openai/chat`, {
        method: "POST",
        headers: {
          "Content-Type": "application/json",
        },
        body: JSON.stringify({
          messages: [
            {
              role: "system",
              content:
                "Given a conversation between a user and a system, summarize the memories of the user in first person point of view.",
            },
            { role: "user", content: JSON.stringify(messages2) },
          ],
          model: "gpt-4",
        }),
      });

      const data_ = await response_.json();
      const memory = data_.completion.content;

      try {
        axios.post("/api/memory_bank/save", {
          personId: personIdFrom,
          memory: memory,
        });
      } catch (e) {
        console.log(e);
      }
    }
  };

  return (
    <>
      <h2>
        {personIdFrom} talking to {personIdTo}
      </h2>

      <div>
        <p>memory bank for {personIdFrom} (you)</p>
        {personFromMemories &&
          personFromMemories.map((m) => <li>{m.memory}</li>)}
      </div>

      <div>
        <p>memory bank for {personIdTo} (other)</p>
        {personToMemories && personToMemories.map((m) => <li>{m.memory}</li>)}
      </div>

      <div>
        <h2>Storytime (pipeline 1)</h2>
        {messages &&
          messages.map((m) => {
            return (
              <>
                <h3>{m.role}</h3>
                <p>{m.content}</p>
              </>
            );
          })}
      </div>

      <button
        onClick={() => {
          handleSwitchPipeline(pipeline);
          setPipeline(!pipeline);
        }}
      >
        switch to pipeline {pipeline ? 2 : 1}
      </button>

      <div>
        <h2>Question time (pipeline 2)</h2>
        {messages2 &&
          messages2.map((m) => {
            return (
              <>
                <h3>{m.role}</h3>
                <p>{m.content}</p>
              </>
            );
          })}
      </div>

      <div>
        <h2>Send Message</h2>
        <div>
          <button onClick={handleStartRecording}>Start Recording</button>
          <button onClick={handleStopRecording}>Stop Recording</button>
          <button
            onClick={() => {
              handleSubmitRecording(pipeline);
            }}
          >
            Submit
          </button>
          <audio src={audio && audio} controls></audio>
          <input
            id="audioFile"
            type="file"
            onChange={(e) => {
              setAudioFile(e.target.files[0]);
            }}
          />
        </div>

        {transcription && (
          <div>
            <h3>Transcription:</h3>
            <p>{transcription}</p>
          </div>
        )}
      </div>
    </>
  );
}
