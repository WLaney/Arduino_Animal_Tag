- Mode is selected by the FM2:0 bits in the FIFO_CTRL register
- FIFO threshold/empty/overrun events available in FIFO_SRC register
  * FIFO_SRC(EMPTY)  - 1 if no samples available.
  * FIFO_SRC(FTH)    - 1 if data arrives and FIFO_SRC(FSS4:0) is greater than
                       the thresholds configured in FIFO_CTRL(2Eh).
  * FIFO_SRC(OVRN)   - 1 if a FIFO slot is overwritten

- FIFO data is read through OUT_{X,Y,Z}_{L,H}. Every time these registers
  are read, the next oldest values are moved into the FIFO register.
- Burst reading is possible, as addresses after OUT_Z loop back to OUT_X.
  In other words, requesting (6*31) bytes of data will bequeath the next
  31 reads.
