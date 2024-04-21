const mongoose = require("mongoose");

/**
 * Mongoose schema defining the Image model.
 *
 */
const ImageSchema = new mongoose.Schema({
  url: {
    type: String,
    required: true,
  },
});

module.exports = mongoose.models.Memory || mongoose.model("Image", ImageSchema);
