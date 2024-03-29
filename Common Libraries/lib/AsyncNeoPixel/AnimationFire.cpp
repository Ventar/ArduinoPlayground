#include <AnimationFire.h>

namespace leds {

void AnimationFire::setPixelHeatColor(int Pixel, byte temperature) {
  // Scale 'heat' down from 0-255 to 0-191
  byte t192 = round((temperature / 255.0) * 191);

  // calculate ramp up from
  byte heatramp = t192 & 0x3F; // 0..63
  heatramp <<= 2;              // scale up to 0..252

  // figure out which third of the spectrum we're in:
  if (t192 > 0x80) { // hottest
    setPixel(Pixel, 255, 255, heatramp);
  } else if (t192 > 0x40) { // middle
    setPixel(Pixel, 255, heatramp, 0);
  } else { // coolest
    setPixel(Pixel, heatramp, 0, 0);
  }
}

void AnimationFire::process() {
  int Cooling = 55;
  int Sparking = 120;
  byte *heat = new byte[strip->numPixels()];
  int cooldown;

  // Step 1.  Cool down every cell a little
  for (int i = 0; i < strip->numPixels(); i++) {
    cooldown = random(0, ((Cooling * 10) / strip->numPixels()) + 2);

    if (cooldown > heat[i]) {
      heat[i] = 0;
    } else {
      heat[i] = heat[i] - cooldown;
    }
  }

  // Step 2.  Heat from each cell drifts 'up' and diffuses a little
  for (int k = strip->numPixels() - 1; k >= 2; k--) {
    heat[k] = (heat[k - 1] + heat[k - 2] + heat[k - 2]) / 3;
  }

  // Step 3.  Randomly ignite new 'sparks' near the bottom
  if (random(255) < Sparking) {
    int y = random(7);
    heat[y] = heat[y] + random(160, 255);
    // heat[y] = random(160,255);
  }

  // Step 4.  Convert heat to LED colors
  uint16_t pixel;
  for (int j = -5; j < strip->numPixels() - 5; j++) {
    if (j < 0) {
      pixel = strip->numPixels() - j;
    } else {
      pixel = j;
    }

    setPixelHeatColor(j, heat[j]);
  }

  show();
  delete[] heat;
}

} // namespace leds