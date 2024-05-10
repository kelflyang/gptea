import dbConnect from "../../lib/mongodb";
import Effect from "../../lib/models/Effect";
import mime from "mime";
import path from "path";

const fs = require("fs");

export default async function handler(req, res) {
  try {
    await dbConnect();
    const effect = await Effect.findOne({}, {}, { sort: { _id: -1 } });
    res.status(200).json(effect);
  } catch (error) {
    console.error("Error writing to database:", error);
    return res.status(500).json({ message: "Internal Server Error" });
  }
}
