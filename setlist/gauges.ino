// Define meter size


void painelBateria()
{
    ttgo->tft->setTextSize(1);
    // Meter outline
    ttgo->tft->fillRect(0, 0, M_SIZE * 239, M_SIZE * 131, TFT_WHITE);
    ttgo->tft->fillRect(1, M_SIZE * 3, M_SIZE * 234, M_SIZE * 125, TFT_WHITE);

    ttgo->tft->setTextColor(TFT_BLACK);  // Text colour

    // Draw ticks every 5 degrees from -50 to +50 degrees (100 deg. FSD swing)
    for (int i = -50; i < 51; i += 5) {
        // Long scale tick length
        int tl = 15;

        // Coodinates of tick to draw
        float sx = cos((i - 90) * 0.0174532925);
        float sy = sin((i - 90) * 0.0174532925);
        uint16_t x0 = sx * (M_SIZE * 100 + tl) + M_SIZE * 120;
        uint16_t y0 = sy * (M_SIZE * 100 + tl) + M_SIZE * 150;
        uint16_t x1 = sx * M_SIZE * 100 + M_SIZE * 120;
        uint16_t y1 = sy * M_SIZE * 100 + M_SIZE * 150;

        // Coordinates of next tick for zone fill
        float sx2 = cos((i + 5 - 90) * 0.0174532925);
        float sy2 = sin((i + 5 - 90) * 0.0174532925);
        int x2 = sx2 * (M_SIZE * 100 + tl) + M_SIZE * 120;
        int y2 = sy2 * (M_SIZE * 100 + tl) + M_SIZE * 150;
        int x3 = sx2 * M_SIZE * 100 + M_SIZE * 120;
        int y3 = sy2 * M_SIZE * 100 + M_SIZE * 150;

        // Yellow zone limits
        //if (i >= -50 && i < 0) {
        //  ttgo->tft->fillTriangle(x0, y0, x1, y1, x2, y2, TFT_YELLOW);
        //  ttgo->tft->fillTriangle(x1, y1, x2, y2, x3, y3, TFT_YELLOW);
        //}

        // Green zone limits
        if (i >= 0 && i < 25) {
            ttgo->tft->fillTriangle(x0, y0, x1, y1, x2, y2, TFT_GREEN);
            ttgo->tft->fillTriangle(x1, y1, x2, y2, x3, y3, TFT_GREEN);
        }

        // Orange zone limits
        if (i >= 25 && i < 50) {
            ttgo->tft->fillTriangle(x0, y0, x1, y1, x2, y2, TFT_ORANGE);
            ttgo->tft->fillTriangle(x1, y1, x2, y2, x3, y3, TFT_ORANGE);
        }

        // Short scale tick length
        if (i % 25 != 0) tl = 8;

        // Recalculate coords incase tick lenght changed
        x0 = sx * (M_SIZE * 100 + tl) + M_SIZE * 120;
        y0 = sy * (M_SIZE * 100 + tl) + M_SIZE * 150;
        x1 = sx * M_SIZE * 100 + M_SIZE * 120;
        y1 = sy * M_SIZE * 100 + M_SIZE * 150;

        // Draw tick
        ttgo->tft->drawLine(x0, y0, x1, y1, TFT_BLACK);

        // Check if labels should be drawn, with position tweaks
        if (i % 25 == 0) {
            // Calculate label positions
            x0 = sx * (M_SIZE * 100 + tl + 10) + M_SIZE * 120;
            y0 = sy * (M_SIZE * 100 + tl + 10) + M_SIZE * 150;
            switch (i / 25) {
            case -2: ttgo->tft->drawCentreString("0", x0 + 4, y0 - 4, 1); break;
            case -1: ttgo->tft->drawCentreString("6", x0 + 2, y0, 1); break;
            case 0: ttgo->tft->drawCentreString("12", x0, y0, 1); break;
            case 1: ttgo->tft->drawCentreString("18", x0, y0, 1); break;
            case 2: ttgo->tft->drawCentreString("24", x0 - 2, y0 - 4, 1); break;
            }
        }

        // Now draw the arc of the scale
        sx = cos((i + 5 - 90) * 0.0174532925);
        sy = sin((i + 5 - 90) * 0.0174532925);
        x0 = sx * M_SIZE * 100 + M_SIZE * 120;
        y0 = sy * M_SIZE * 100 + M_SIZE * 150;
        // Draw scale arc, don't draw the last part
        if (i < 50) ttgo->tft->drawLine(x0, y0, x1, y1, TFT_BLACK);
    }

    ttgo->tft->drawString("V", M_SIZE * (3 + 230 - 40), M_SIZE * (119 - 20), 2); // Units at bottom right
    ttgo->tft->drawCentreString("BATERIA", M_SIZE * 120, M_SIZE * 75, 2); // Comment out to avoid font 4
    // ttgo->tft->drawRect(2, M_SIZE * 3, M_SIZE * 236, M_SIZE * 126, TFT_BLACK); // Draw bezel line

    ponteiroBateria(0, 0); // Put meter needle at 0
}


void ponteiroBateria(int value, byte ms_delay)
{
    ttgo->tft->setTextSize(1);
    ttgo->tft->setTextColor(TFT_BLACK, TFT_WHITE);
    char buf[8]; dtostrf((value/4), 4, 0, buf);
    ttgo->tft->drawRightString( String(truncate(value/4,2)), 33, M_SIZE * (119 - 20), 2);
    if (value < -10) value = -10; // Limit value to emulate needle end stops
    if (value > 110) value = 110;

    // Move the needle until new value reached
    while (!(value == old_analog)) {
        if (old_analog < value) old_analog++;
        else old_analog--;

        if (ms_delay == 0) old_analog = value; // Update immediately if delay is 0

        float sdeg = map(old_analog, -10, 110, -150, -30); // Map value to angle
        // Calculate tip of needle coords
        float sx = cos(sdeg * 0.0174532925);
        float sy = sin(sdeg * 0.0174532925);

        // Calculate x delta of needle start (does not start at pivot point)
        float tx = tan((sdeg + 90) * 0.0174532925);

        // Erase old needle image
        ttgo->tft->drawLine(M_SIZE * (120 + 24 * ltx) - 1, M_SIZE * (150 - 24), osx - 1, osy, TFT_WHITE);
        ttgo->tft->drawLine(M_SIZE * (120 + 24 * ltx), M_SIZE * (150 - 24), osx, osy, TFT_WHITE);
        ttgo->tft->drawLine(M_SIZE * (120 + 24 * ltx) + 1, M_SIZE * (150 - 24), osx + 1, osy, TFT_WHITE);

        // Re-plot text under needle
        ttgo->tft->setTextColor(TFT_BLACK, TFT_WHITE);
        ttgo->tft->drawCentreString("BATERIA", M_SIZE * 120, M_SIZE * 75, 2); // // Comment out to avoid font 4

        // Store new needle end coords for next erase
        ltx = tx;
        osx = M_SIZE * (sx * 98 + 120);
        osy = M_SIZE * (sy * 98 + 150);

        // Draw the needle in the new postion, magenta makes needle a bit bolder
        // draws 3 lines to thicken needle
        ttgo->tft->drawLine(M_SIZE * (120 + 24 * ltx) - 1, M_SIZE * (150 - 24), osx - 1, osy, TFT_RED);
        ttgo->tft->drawLine(M_SIZE * (120 + 24 * ltx), M_SIZE * (150 - 24), osx, osy, TFT_MAGENTA);
        ttgo->tft->drawLine(M_SIZE * (120 + 24 * ltx) + 1, M_SIZE * (150 - 24), osx + 1, osy, TFT_RED);

        // Slow needle down slightly as it approaches new postion
        if (abs(old_analog - value) < 10) ms_delay += ms_delay / 5;

        // Wait before next update
        delay(ms_delay);
    }
}

