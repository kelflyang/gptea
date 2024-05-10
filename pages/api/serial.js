const SerialPort = require("serialport");
const Readline = require("@serialport/parser-readline");
const port = new SerialPort("/dev/cu.usbserial-14110", { baudRate: 115200 });
const parser = port.pipe(new Readline({ delimiter: "\n" }));
// Read the port data
port.on("open", () => {
  console.log("serial port open");
});
parser.on("data", (data) => {
  console.log("got word from arduino:", data);
});
