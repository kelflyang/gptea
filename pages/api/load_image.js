import dbConnect from "../../lib/mongodb";
import Image from "../../lib/models/Image";
import mime from "mime";
import fs from "fs";

export default async function handler(req, res) {
  try {
    await dbConnect();
    const image = await Image.findOne({}, {}, { sort: { _id: -1 } });

    // Replace 'destinationUrl' with the URL you want to redirect to
    const destinationUrl = image.url;

    // Set the appropriate HTTP status code for redirection (e.g., 301 or 302)
    res.status(302);

    // Set the 'Location' header to the destination URL
    res.setHeader("Location", destinationUrl);

    // End the response
    res.end();
  } catch (error) {
    console.error("Error writing to database:", error);
    return res.status(500).json({ message: "Internal Server Error" });
  }
}
