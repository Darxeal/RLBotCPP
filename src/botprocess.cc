#include "botprocess.h"

#include "platform.h"

namespace rlbot {
void BotProcess::Start() {
  running = true;
  thread = std::thread(&BotProcess::BotThread, this);
}

void BotProcess::Stop() {
  running = false;
  thread.join();
}

void BotProcess::BotThread() {
  float lasttime = 0;

  // Don't start before the interface is ready.
  while (!Interface::IsInitialized()) {
    platform::SleepMilliseconds(1);
  }

  while (running) {
    ByteBuffer flatbufferData = Interface::UpdateLiveDataPacketFlatbuffer();

    // Don't try to read the packets when they are very small.
    if (flatbufferData.size > 4) {
      const rlbot::flat::GameTickPacket *gametickpacket =
          flatbuffers::GetRoot<rlbot::flat::GameTickPacket>(flatbufferData.ptr);

      // Only run the bot when we recieve a new packet.
      if (lasttime != gametickpacket->gameInfo()->secondsElapsed()) {
        ByteBuffer fieldInfoData = Interface::UpdateFieldInfoFlatbuffer();
        ByteBuffer ballPredictionData = Interface::GetBallPrediction();

        // The fieldinfo or ball prediction might not have been set up by the
        // framework yet.
        if (fieldInfoData.size > 4 && ballPredictionData.size > 4) {
          const rlbot::flat::FieldInfo *fieldinfo =
              flatbuffers::GetRoot<rlbot::flat::FieldInfo>(fieldInfoData.ptr);
          const rlbot::flat::BallPrediction *ballprediction =
              flatbuffers::GetRoot<rlbot::flat::BallPrediction>(
                  ballPredictionData.ptr);

          int status = Interface::SetBotInput(
              bot->GetOutput(gametickpacket, fieldinfo, ballprediction),
              bot->index);

          Interface::Free(fieldInfoData.ptr);
          Interface::Free(ballPredictionData.ptr);
        }

        lasttime = gametickpacket->gameInfo()->secondsElapsed();
      } else {
        platform::SleepMilliseconds(1);
      }
    } else {
      platform::SleepMilliseconds(100);
    }

    Interface::Free(flatbufferData.ptr);
  }
}
} // namespace rlbot
