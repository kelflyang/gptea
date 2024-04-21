import React, { useEffect, useState } from "react";

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

  return (
    <>
      <h2>
        {personIdFrom} talking to {personIdTo}
      </h2>
    </>
  );
}
