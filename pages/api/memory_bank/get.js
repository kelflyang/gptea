import dbConnect from "../../../lib/mongodb";
import Memory from "../../../lib/models/Memory";

export default async function handler(req, res) {
  if (req.method !== "POST") {
    return res.status(405).json({ message: "Method Not Allowed" });
  }

  try {
    await dbConnect();
    const memories = await Memory.find({ personId: req.body.personId });
    return res
      .status(200)
      .json({ message: "Successfully retrieved memories", memories: memories });
  } catch (error) {
    console.error("Error writing to database:", error);
    return res.status(500).json({ message: "Internal Server Error" });
  }
}
