import React, { useState, useEffect, useRef } from "react";
import styles from "../styles/Home.module.css";
import axios from "axios";

const API_KEY = "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";

const AudioTranscriber = ({ personId }) => {
  const [memories, setMemories] = useState(null);
  const mediaRecorder = useRef(null);
  const [stream, setStream] = useState(null);
  const [audioChunks, setAudioChunks] = useState([]);
  const [audio, setAudio] = useState(null);
  const [audioFile, setAudioFile] = useState(null);
  const [transcription, setTranscription] = useState("");
  const [audioBlob, setAudioBlob] = useState("");
  const [story, setStory] = useState("");
  const [imageDescription, setImageDescription] = useState("");
  const [imageLink, setImageLink] = useState("");
  const [keywords, setKeywords] = useState("");

  const fetchMemories = async (personId) => {
    try {
      const response = await axios.post("/api/memory_bank/get", { personId });
      console.log(response.data.memories);
      setMemories(response.data.memories);
    } catch (error) {
      console.error("Error fetching data:", error);
    }
  };

  useEffect(() => {
    fetchMemories(personId);
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
        // const keywords = await generateKeywords(data.transcription);
        // setKeywords(keywords);
        const story = await generateStort(data.transcription);
        setStory(story);
        const description = await getImageDescription(story);
        setImageDescription(description);
        const imageLink = await generateImage(description);
        // console.log("image link ", imageLink);
        setImageLink(imageLink);
        const audio = await generateSpeech(story);
      };
      reader.readAsDataURL(audioBlob);
    } catch (error) {
      console.log(error);
    } finally {
    }
  };

  // const handleFileChange = async (event) => {
  //   const file = event.target.files[0];

  //   try {
  //     const transcriptionResponse = await transcribeAudio(file);
  //     setTranscription(transcriptionResponse.text);
  //     const keywords = await generateKeywords(transcriptionResponse.text);
  //     setKeywords(keywords);
  //     const story = await generateStort(keywords);
  //     setStory(story);
  //     const description = await getImageDescription(keywords);
  //     setImageDescription(description);
  //     const imageLink = await generateImage(description);
  //     // console.log("image link ", imageLink);
  //     setImageLink(imageLink);
  //     const audio = await generateSpeech(story);
  //     // const url = URL.createObjectURL(audio.buffer);
  //     // console.log("url ", url);
  //     // setAudioURL(url);
  //   } catch (error) {
  //     console.error("Error transcribing audio:", error);
  //   }
  // };

  const transcribeAudio = async (file) => {
    // Prepare the file for transmission to the OpenAI API
    const formData = new FormData();
    formData.append("file", file);
    formData.append("model", "whisper-1");

    try {
      // Make a POST request to the OpenAI API endpoint
      const response = await fetch(
        "https://api.openai.com/v1/audio/transcriptions",
        {
          method: "POST",
          body: formData,
          headers: {
            // Set necessary headers, such as API key, content type, etc.
            Authorization: `Bearer ${API_KEY}`,
          },
        }
      );

      if (!response.ok) {
        throw new Error("Failed to transcribe audio");
      }

      // Parse the JSON response
      const data = await response.json();
      return data; // Assuming the API returns transcription in the response
    } catch (error) {
      throw error;
    }
  };

  const generateKeywords = async (transcription) => {
    try {
      const response = await axios.post("/api/openai/keywords", {
        transcription,
      });
      if (!response.data.keywords) {
        throw new Error("Failed to call API");
      }

      return response.data.keywords;
    } catch (error) {
      console.error("Error calling API:", error);
      throw error;
    }
  };

  const generateStort = async (keywords) => {
    try {
      const response = await axios.post("/api/openai/story", {
        keywords,
        relation: "grandfather",
      });
      if (!response.data.story) {
        throw new Error("Failed to call API");
      }

      return response.data.story;
    } catch (error) {
      console.error("Error calling API:", error);
      throw error;
    }
  };

  const getImageDescription = async (story) => {
    try {
      const response = await axios.post("/api/openai/image_prompt", { story });
      if (!response.data.imageDescription) {
        throw new Error("Failed to call API");
      }

      return response.data.imageDescription;
    } catch (error) {
      console.error("Error calling API:", error);
      throw error;
    }
  };

  const generateImage = async (imageDescription) => {
    try {
      const response = await axios.post(
        "https://api.openai.com/v1/images/generations",
        {
          prompt: imageDescription,
          size: "256x256",
          response_format: "url",
        },
        {
          headers: {
            Authorization: `Bearer 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`,
            "Content-Type": "application/json",
          },
        }
      );
      return response.data.data[0].url;
    } catch (error) {
      console.error("Error calling API:", error);
      throw error;
    }
  };

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
            Authorization: `Bearer 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`,
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

  const handleSaveMemory = async () => {
    try {
      axios.post("/api/memory_bank/save", {
        personId: personId,
        memory: transcription,
      });
    } catch (e) {
      console.log(e);
    }

    await fetchMemories(personId);
  };

  return (
    <div className={styles.submitterContainer}>
      <div>
        <h2>{personId}'s Memory Bank</h2>
        {memories && memories.map((m) => <li>{m.memory}</li>)}
      </div>
      <div>
        <h2>Submit New Memories for {personId}</h2>
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

        <button onClick={handleSaveMemory}>save transcription as memory</button>

        {keywords && (
          <div>
            <h3>Keywords:</h3>
            <p>{keywords}</p>
          </div>
        )}

        {story && (
          <div>
            <h3>Story Generation:</h3>
            <p>{story}</p>
          </div>
        )}

        {imageDescription && (
          <div>
            <h3>Image Description:</h3>
            <p>{imageDescription}</p>
          </div>
        )}

        {imageLink && <img src={imageLink}></img>}
      </div>
    </div>
  );
};

export default AudioTranscriber;
