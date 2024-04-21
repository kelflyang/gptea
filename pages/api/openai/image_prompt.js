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
      content:
        "Given a story, return a concise image description for an image generator. Do not mention it is an image or comment on the quality of the description. You have specify it is a sepia or black and white image.",
    },
  ];

  messages.push({
    role: "user",
    content: req.body.story,
  });

  try {
    // Call the OpenAI completion API
    const response = await openai.chat.completions.create({
      messages,
      model: "gpt-4",
    });

    const imageDescription = response.choices[0].message.content;
    return res.status(200).json({ imageDescription });
  } catch (error) {
    console.error("Error generating completion:", error);
    return res.status(500).json({ message: "Internal Server Error" });
  }
}
