import OpenAI from "openai";

const openai = new OpenAI({
  apiKey: process.env.OPENAI_API_KEY,
});

export default async function handler(req, res) {
  if (req.method !== "POST") {
    return res.status(405).json({ message: "Method Not Allowed" });
  }

  try {
    // Call the OpenAI completion API
    const response = await openai.chat.completions.create(req.body);
    const completion = response.choices[0].message;
    return res.status(200).json({ completion });
  } catch (error) {
    console.error("Error generating completion:", error);
    return res.status(500).json({ message: "Internal Server Error" });
  }
}
