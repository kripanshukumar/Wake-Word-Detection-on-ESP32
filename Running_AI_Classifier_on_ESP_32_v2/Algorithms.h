void smooth(float x){
  Sequence = Sequence<<1;
  inputSeuence = inputSeuence<<1;
  bitClear(Sequence,6);
  bitClear(inputSeuence,6);
  bitSet(Sequence,5);
  bitSet(inputSeuence,5);
  
  if(x > threshold){
    bitSet(inputSeuence,0);
  }
  else{
    bitClear(inputSeuence, 0);
  }
  int i = 0;
  for(int i = 0; i < 16; i++){
    if((valid[i]+32) == inputSeuence){
      Serial.print("Valid");
      bitSet(Sequence,0);
      break;
    }
  }
  if( i>= 12){
    bitClear(Sequence,0);
  }

  Serial.print(" Sequence: ");Serial.print(Sequence);Serial.print('\t');Serial.print(Sequence,BIN);
  Serial.print(" Input Sequence: ");Serial.print(inputSeuence);Serial.print('\t');Serial.println(inputSeuence,BIN);
}

float averageProb(float x){
//  Accumulator -= circularBuffer[bufferPointer];
//  Accumulator += x;
//  circularBuffer[bufferPointer] = x;
//  average = Accumulator/AVERAGING_SAMPLES;
//  bufferPointer++;
//  if(bufferPointer >= AVERAGING_SAMPLES -1 ){
//    bufferPointer = 0;
//  }
  average = 0.8*average + 0.2*x;
  Serial.print("Average: ");Serial.println(average);
  return average;
}
