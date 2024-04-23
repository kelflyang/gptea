import Head from "next/head";
import styles from "../styles/Home.module.css";
import React from "react";
import Chat from "../components/Chat";
import axios from "axios";

export default function MemoryChat() {
  return (
    <div className={styles.container}>
      <Head>
        <title>gptea</title>
        <link rel="icon" href="/favicon.ico" />
      </Head>

      <main>
        <div className={styles.container}>
          <div>
            <Chat personIdFrom={"p2"} personIdTo={"p1"} />
          </div>
        </div>
      </main>
    </div>
  );
}
