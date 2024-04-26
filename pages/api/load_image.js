import dbConnect from "../../lib/mongodb";
import Image from "../../lib/models/Image";
import mime from "mime";
import path from "path";

const fs = require("fs");
const pngToJpeg = require("png-to-jpeg");

export default async function handler(req, res) {
  try {
    await dbConnect();
    const image = await Image.findOne({}, {}, { sort: { _id: -1 } });
    const imagePath = "test.jpg";
    const contentType = mime.getType(imagePath);

    const buffer = Buffer.from(image.buffer, "base64");
    pngToJpeg({ quality: 90 })(buffer).then((output) => {
      // Set response headers
      res.setHeader("Content-Type", contentType);
      // res.setHeader("Cache-Control", "public, max-age=86400"); // Cache the image for 1 day

      // Send the Base64-encoded image data as the response
      res.send(output);
    });
  } catch (error) {
    console.error("Error writing to database:", error);
    return res.status(500).json({ message: "Internal Server Error" });
  }
}
