import Head from "next/head";
import styles from "../styles/Home.module.css";
import React from "react";
import AudioTranscriber from "../components/AudioTranscriber";

export default function MemorySubmission() {
  return (
    <div className={styles.container}>
      <Head>
        <title>gptea</title>
        <link rel="icon" href="/favicon.ico" />
      </Head>

      <main>
        <div className={styles.container}>
          <div>
            <AudioTranscriber personId={"p1"}></AudioTranscriber>
          </div>
          <div>
            <AudioTranscriber personId={"p2"}></AudioTranscriber>
          </div>
        </div>
      </main>
    </div>
  );
}
