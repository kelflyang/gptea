const mongoose = require("mongoose");

/**
 * Mongoose schema defining the Image model.
 *
 */
const ImageSchema = new mongoose.Schema({
  url: String,
});

module.exports = mongoose.models.Image || mongoose.model("Image", ImageSchema);
