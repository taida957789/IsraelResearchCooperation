#include <AESLib.h>

void debug(char* topic, char* text)
{
  Serial.print("[Debug]");
  Serial.print(topic);
  Serial.print(" = ");
  Serial.println(text);
}

void debug(char* topic, int val)
{
  Serial.print("[Debug]");
  Serial.print(topic);
  Serial.print(" = ");
  Serial.println(val);
}

char* padding(char* data, int dataLen, int* outBlockCount)
{
  int bytesEachBlock = 128 / 8; // block size
  int paddCount = bytesEachBlock - (dataLen % bytesEachBlock);
  int newLen = dataLen;
  if ( paddCount < 16)
    newLen = dataLen + paddCount;
  *outBlockCount = newLen / bytesEachBlock;
  char * newData = (char*)malloc(newLen + 1);
  memcpy(newData, data, dataLen);
  memset(newData + dataLen, 5, newLen - dataLen);
  newData[newLen] = 0;
  return newData;
}

void unpadding(char* padded, int blockCount)
{
  for (int i = 15; i >= 0; --i)
  {
    if (padded[16 * (blockCount - 1) + i] == 0x5)
      padded[16 * (blockCount - 1) + i] = 0;
    else
      break;
  } 
}

int aes256_enc(char* plaintext, unsigned int len, uint8_t* key, char* &outCipherText)
{
  int blockCount = 0;

  char * padded = padding(plaintext, len, &blockCount);

  for (int i = 0 ; i < blockCount; ++i)
  {
    aes256_enc_single(key, padded + 16 * i);
  }
  outCipherText = padded;
  return 16 * blockCount;
}

int aes256_dec(char* ciphertext, unsigned int len, uint8_t* key, char* &outPlainText)
{
  int blockCount = 0;
  char * padded = padding(ciphertext, len, &blockCount);

  for (int i = 0 ; i < blockCount; ++i)
  {
    aes256_dec_single(key, padded + 16 * i);
  }

  unpadding(padded, blockCount);

  outPlainText = padded;
  return strlen(padded);
}


void setup() {
  Serial.begin(19200);
}

void loop() {
  uint8_t key[] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15};
  char plaintext[] = "0123456789012345qeqweqweqwqeqweqeqweqweqeqee";
  int length_plaintext = strlen(plaintext);
  char* encrypted = NULL;
  char* decrypted = NULL;
  debug("plaintext", plaintext);
  debug("length of plaintext", length_plaintext);
  int len_ciphertext = aes256_enc(plaintext, length_plaintext, key, encrypted);
  debug("encrypted", encrypted);
  aes256_dec(encrypted, len_ciphertext, key, decrypted);
  debug("decrypted", decrypted);
  free(encrypted);
  free(decrypted);
  delay(5000);
}
