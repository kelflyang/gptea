import OpenAI from "openai";

const openai = new OpenAI({
  apiKey: process.env.OPENAI_API_KEY,
});

export default async function handler(req, res) {
  if (req.method !== "POST") {
    return res.status(405).json({ message: "Method Not Allowed" });
  }
  const messages = [
    {
      role: "system",
      content: "Given a story, return a list of keywords",
    },
  ];

  messages.push({
    role: "user",
    content: req.body.transcription,
  });

  try {
    // Call the OpenAI completion API
    const response = await openai.chat.completions.create({
      messages,
      model: "gpt-4",
    });

    const keywords = response.choices[0].message.content;
    return res.status(200).json({ keywords });
  } catch (error) {
    console.error("Error generating completion:", error);
    return res.status(500).json({ message: "Internal Server Error" });
  }
}
