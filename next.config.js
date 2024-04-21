const dotenv = require("dotenv");

dotenv.config();

module.exports = {
  env: {
    OPENAI_API_KEY: process.env.OPENAI_API_KEY,
  },
};
