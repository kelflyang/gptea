import OpenAI from "openai";
import Image from "../../../lib/models/Image";

const openai = new OpenAI({
  apiKey: process.env.OPENAI_API_KEY,
});

export default async function handler(req, res) {
  if (req.method !== "POST") {
    return res.status(405).json({ message: "Method Not Allowed" });
  }

  try {
    // Call the OpenAI completion API
    const response = await openai.images.generate({
      prompt: req.body.prompt,
      size: "256x256",
    });
    console.log("response ", response);
    const image = response.data[0].url;

    return res.status(200).json({ image });
  } catch (error) {
    console.error("Error generating completion:", error);
    return res.status(500).json({ message: "Internal Server Error" });
  }
}
