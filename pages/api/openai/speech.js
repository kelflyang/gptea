import OpenAI from "openai";

const openai = new OpenAI({
  apiKey: process.env.NEXT_PUBLIC_OPENAI,
});

export default async function handler(req, res) {
  if (req.method !== "POST") {
    return res.status(405).json({ message: "Method Not Allowed" });
  }

  try {
    // Call the OpenAI completion API
    const response = await openai.audio.speech.create({
      model: "tts-1",
      input: req.body.text,
      voice: "nova",
    });

    console.log("response ", response);
    const buffer = Buffer.from(await response.arrayBuffer());

    return res.status(200).json({ buffer });
  } catch (error) {
    console.error("Error generating completion:", error);
    return res.status(500).json({ message: "Internal Server Error" });
  }
}
