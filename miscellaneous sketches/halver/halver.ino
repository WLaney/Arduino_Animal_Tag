const short buffer_size = 1000;
static_assert(buffer_size % 2 == 0, "Buffer should be even in size");

class Buffer {
  private:
    int size;

  public:
    Buffer(int size) : size{size} { }

    void pop(byte *a, int n) {
      if (n > size) {
        Serial.print(F("Tried to pull "));
        Serial.print(n);
        Serial.print(F(" samples from a buffer of size "));
        Serial.println(size);
      }
      while (--n >= 0) {
        a[n] = byte{--size};
      }
    }
};

void setup() {
  Serial.begin(9600);
  Buffer in(buffer_size);
  byte out[buffer_size / 2];

  Serial.println("Starting...");
  long time = millis();
  pop_and_downscale(in, out, buffer_size);
  time = millis() - time;

  Serial.print(time);
  Serial.println(F(" ms taken"));
}

void loop() {

}

/*
   Remove every other int in an array of size n.
   n must be even
*/
void downscale(byte *a, int n) {
  for (int s = 0; s < n / 2; s++) {
    a[s] = a[s * 2];
  }
}

/*
   We are given an input buffer and an output buffer; the output buffer
   is half the size of the input buffer (the input buffer's size is
   even). Move every other sample of the input into the output in as
   few reads as possible with constant-time space complexity.
*/
void pop_and_downscale(Buffer in, byte *out, int size) {
  // Number of samples left to copy
  int left = size;
  // Amount of space left in the buffer
  int space = size / 2;
  // Small temporary buffer
  byte temp[2];

  while (left > 0) {
    byte *op = out + size / 2 - space;
    if (left & 1) {
      left /= 2;
      in.pop(op, left);
      for (int s = 0; s < left; s++) {
        op[s] = op[s * 2];
      }
    } else {
      in.pop(temp, 2);
      *op = byte{temp[0]};
      left -= 2;
      space--;
    }
  }
}
