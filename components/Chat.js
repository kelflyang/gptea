import React, { useEffect, useState, useRef } from "react";
import axios from "axios";

export default function Chat({ personIdFrom, personIdTo }) {
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

  useEffect(async () => {
    const fromMemories = await fetchMemories(personIdFrom);
    const toMemories = await fetchMemories(personIdTo);

    setPersonFromMemories(fromMemories);
    setPersonToMemories(toMemories);

    const response = await axios.post("/api/openai/chat", {
      messages: [
        {
          role: "system",
          content: `The goal is connect two people through each other’s stories. Let’s call the two people User A and User B. You will be interacting with User A only. Your role is to take in a list of entries from User A and a list of entries from User B, based on these entries find clues of connection between User A and User B. Generate one question to be asked to User A. User A doesn’t know what User B has written. Ask the question in a way so that User A’s response will lead to a connection with one of User B’s entries.
  
          Here is the list of stories already shared by User A.
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
          
          Here is the list of stories already shared by User B:
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
          
          Please respond with the entry you have chosen from User B’s list and the generated question with the following format:

          {"entry": “—----------------”,
          "question": “—----------------”}
          
          `,
        },
      ],
      model: "gpt-4",
    });

    const info = JSON.parse(response.data.completion.content);
    console.log(info);
    setInfo(info);

    const system = `Have a conversation with the user starting with this question:

   ${info.question}
    
    Ask the questions such that it is relevant to this memory from another person: ${info.entry}
    
    Then after three exchanges do as follows:
    
    You are also given a transcript of a memory provided by the user's grandfather: ${info.entry} To conclude the conversation, relay the memory to the user in a narrative story. You can add in some details and embellish it.  Do not add any details you don't know to be true.  Refer to the user's grandpa as "your grandpa". Do not restate the memory or introduce the story. Just tell the story. `;

    setMessages([{ role: "system", content: system }]);
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

  const handleSubmitRecording = async () => {
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

        setMessages([
          ...messages,
          { role: "user", content: data.transcription },
        ]);

        // get chat completion
        const response_ = await fetch(`/api/openai/chat`, {
          method: "POST",
          headers: {
            "Content-Type": "application/json",
          },
          body: JSON.stringify({
            messages: [
              ...messages,
              { role: "user", content: data.transcription },
            ],
            model: "gpt-4",
          }),
        });

        const data_ = await response_.json();

        setMessages([...messages, data_.completion]);
      };
      reader.readAsDataURL(audioBlob);
    } catch (error) {
      console.log(error);
    } finally {
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
        <h2>chat</h2>
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

      <div>
        <h2>Send Message</h2>
        <div>
          <button onClick={handleStartRecording}>Start Recording</button>
          <button onClick={handleStopRecording}>Stop Recording</button>
          <button onClick={handleSubmitRecording}>Submit</button>
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
