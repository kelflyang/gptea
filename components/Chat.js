import React, { useEffect, useState, useRef } from "react";
import axios from "axios";

export default function Chat({ personIdFrom, personIdTo }) {
  const [personFromMemories, setPersonFromMemories] = useState(null);
  const [personToMemories, setPersonToMemories] = useState(null);

  const fetchMemories = async (personId) => {
    try {
      const response = await axios.post("/api/memory_bank/get", { personId });
      console.log(response.data.memories);
      if (personId === personIdFrom) {
        setPersonFromMemories(response.data.memories);
      } else {
        setPersonToMemories(response.data.memories);
      }
    } catch (error) {
      console.error("Error fetching data:", error);
    }
  };

  useEffect(() => {
    fetchMemories(personIdFrom);
    fetchMemories(personIdTo);
  }, []);

  const messages = useRef([
    {
      role: "system",
      content: `You are mediating a conversation between the user and another person. You will represent another person and you have a list of their memories: ${
        personFromMemories
          ? "[" +
            personFromMemories
              .map((m) => {
                m.memory;
              })
              .join(", ") +
            "]"
          : "[]"
      }`,
    },
  ]);

  return (
    <>
      <h2>
        {personIdFrom} talking to {personIdTo}
      </h2>

      <div>
        <p>memory bank for {personIdFrom} (you)</p>
        {personFromMemories &&
          personFromMemories.map((m) => <li>{m.memory}</li>)}
      </div>

      <div>
        <p>memory bank for {personIdTo} (other)</p>
        {personToMemories && personToMemories.map((m) => <li>{m.memory}</li>)}
      </div>

      <div>
        <h2>chat</h2>

        <div>{messages.current}</div>
      </div>
    </>
  );
}
