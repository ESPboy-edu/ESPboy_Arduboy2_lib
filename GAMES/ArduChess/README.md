# ArduChess

Play chess on the Arduboy.

<img src="/img/ScreenRecording.gif" width="256" height="128">

Features:
- Full rules of chess (castling, en passant, minor promotion)
- Draw rules implemented and accounted for by AI:
  - stalemate
  - 50 moves
  - insufficient material (FIDE-style)
  - threefold repetition
- Play with a friend, by yourself against AI, or watch the AI self-play
- Customize AI strength and personality (contempt factor)
- Opening book of more than 1000 moves (used by the AI)
- Move history in SAN
- Undo move (up to 12 plies back)
- Save/load games to EEPROM (retaining full repetition and undo history)