const mongoose = require("mongoose");

/**
 * Mongoose schema defining the Memory model.
 *
 */
const MemorySchema = new mongoose.Schema({
  personId: {
    type: String,
    required: true,
  },
  memory: {
    type: String,
    required: true,
  },
});

module.exports =
  mongoose.models.Memory || mongoose.model("Memory", MemorySchema);
