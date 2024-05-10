const mongoose = require("mongoose");

/**
 * Mongoose schema defining the Image model.
 *
 */
const EffectSchema = new mongoose.Schema({
  effect: String,
});

module.exports =
  mongoose.models.Effect || mongoose.model("Effect", EffectSchema);
