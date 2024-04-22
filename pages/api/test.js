import fs from "fs";
import path from "path";
import mime from "mime";

export default function handler(req, res) {
  try {
    // Read the image file (replace 'image.jpg' with your image file name)
    const imagePath = path.join(process.cwd(), "public", "ID.jpeg");
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
    console.error("Error serving image:", error);
    res.status(500).json({ error: "Internal Server Error" });
  }
}
