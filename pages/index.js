import Head from "next/head";
import styles from "../styles/Home.module.css";
import React from "react";
import AudioTranscriber from "../components/AudioTranscriber";
import MicrophoneStreamer from "../components/MicrophoneStreamer";
import Link from "next/link";

export default function Home() {
  return (
    <div className={styles.container}>
      <Head>
        <title>gptea</title>
        <link rel="icon" href="/favicon.ico" />
      </Head>

      <main>
        <Link href={"memory_submission"}>
          Submit new memories to the memory bank here
        </Link>
        <br />
        <Link href={"memory_chat"}>Chat here</Link>
      </main>
    </div>
  );
}
