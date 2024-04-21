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
      content: `You are mediating a conversation between the user and their ${req.body.relation}. Given an input that is a transcription of a memory from the ${req.body.relation}, tell the story to the user. Do not tell it in first person as if you are the grandfather. Do not mention the transcription. You can add in some details and embelish it.`,
    },
  ];

  messages.push({
    role: "user",
    content: req.body.keywords,
  });

  try {
    // Call the OpenAI completion API
    const response = await openai.chat.completions.create({
      messages,
      model: "gpt-4",
    });

    const story = response.choices[0].message.content;
    return res.status(200).json({ story });
  } catch (error) {
    console.error("Error generating completion:", error);
    return res.status(500).json({ message: "Internal Server Error" });
  }
}
