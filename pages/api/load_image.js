import dbConnect from "../../lib/mongodb";
import Image from "../../lib/models/Image";
import mime from "mime";
import path from "path";

const fs = require("fs");

export default async function handler(req, res) {
  try {
    await dbConnect();
    const image = await Image.findOne({}, {}, { sort: { _id: -1 } });
    // console.log(image.buffer);
    const buffer = Buffer.from(image.buffer, "base64");
    const imagePath = path.join(process.cwd(), "public", "test.jpg");

    fs.writeFileSync(imagePath, buffer);
    const imageData = fs.readFileSync(imagePath);

    // Convert image data to a Base64-encoded string
    const base64Image = imageData.toString("base64");

    // Get the MIME type of the image
    const contentType = mime.getType(imagePath);
    console.log(contentType);
    console.log(base64Image.slice(0, 200));

    // Set response headers
    res.setHeader("Content-Type", contentType);
    res.setHeader("Cache-Control", "public, max-age=86400"); // Cache the image for 1 day

    // Send the Base64-encoded image data as the response
    res.send(Buffer.from(base64Image, "base64"));
  } catch (error) {
    console.error("Error writing to database:", error);
    return res.status(500).json({ message: "Internal Server Error" });
  }
}
