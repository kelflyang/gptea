import dbConnect from "../../lib/mongodb";
import Image from "../../lib/models/Image";

export default async function handler(req, res) {
  if (req.method !== "POST") {
    return res.status(405).json({ message: "Method Not Allowed" });
  }

  try {
    await dbConnect();
    const images = await Image.find({});
    return res
      .status(200)
      .json({ message: "Successfully retrieved images", images: images });
  } catch (error) {
    console.error("Error writing to database:", error);
    return res.status(500).json({ message: "Internal Server Error" });
  }
}
