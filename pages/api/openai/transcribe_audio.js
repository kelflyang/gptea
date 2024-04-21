// pages/api/microphone.js

import OpenAI from "openai";
const fs = require("fs");

const openai = new OpenAI({
  apiKey: process.env.OPENAI_API_KEY,
});

export default async function handler(req, res) {
  fs.writeFileSync(
    "/tmp/tmp.webm",
    Buffer.from(req.body.audioBuffer, "base64")
  );

  try {
    const response = await openai.audio.transcriptions.create({
      file: fs.createReadStream("/tmp/tmp.webm"),
      model: "whisper-1",
    });

    // write response to mongodb

    return res.status(200).json({ transcription: response.text });
  } catch (e) {
    console.log(e);
  }
}
