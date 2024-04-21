import dbConnect from "../../../lib/mongodb";
import Memory from "../../../lib/models/Memory";

export default async function handler(req, res) {
  if (req.method !== "POST") {
    return res.status(405).json({ message: "Method Not Allowed" });
  }

  try {
    await dbConnect();
    const newMemory = new Memory(req.body);
    const savedMemory = await newMemory.save();

    return res
      .status(200)
      .json({ message: "Successfully saved memory", memory: savedMemory });
  } catch (error) {
    console.error("Error writing to database:", error);
    return res.status(500).json({ message: "Internal Server Error" });
  }
}
