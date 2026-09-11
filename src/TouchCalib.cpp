#include <SPI.h>
#include <XPT2046_Touchscreen.h>
#include <Preferences.h>
#include <TFT_eSPI.h>
#include "TouchCalib.h"
#include "UIManager.h"

extern XPT2046_Touchscreen touchscreen;  // từ TouchHandler.cpp
extern TFT_eSPI tft;                     // từ Config.cpp / DisplayInit.cpp

#define TOUCH_Z_THRESHOLD 300
#define CALIB_MARGIN       20

struct TouchCalibData { float ax,bx,dx, ay,by,dy; bool valid; };
static TouchCalibData _c = {0,0,0, 0,0,0, false};
static Preferences _prefs;

static bool readAvgRaw(int16_t& rx, int16_t& ry, int n=20, uint32_t tmo=3000) {
    uint32_t t0=millis(); long sx=0, sy=0; int cnt=0;
    while (cnt<n && millis()-t0<tmo) {
        TS_Point p = touchscreen.getPoint();
        if (p.z > TOUCH_Z_THRESHOLD) { sx+=p.x; sy+=p.y; cnt++; delayMicroseconds(800); }
        delay(5);
    }
    if (cnt==0) return false;
    rx=(int16_t)(sx/cnt); ry=(int16_t)(sy/cnt);
    return true;
}

static void crosshair(int cx,int cy,int r,uint16_t c) {
    tft.drawFastHLine(cx-r,cy,r*2+1,c);
    tft.drawFastVLine(cx,cy-r,r*2+1,c);
    tft.drawCircle(cx,cy,r,c);
}

static bool solveAffine(float rx[3], float ry[3], float sx[3], float sy[3],
                         float &ax,float &bx,float &dx, float &ay,float &by,float &dy) {
    float m[3][5];
    for (int i=0;i<3;i++){ m[i][0]=rx[i]; m[i][1]=ry[i]; m[i][2]=1; m[i][3]=sx[i]; m[i][4]=sy[i]; }
    for (int col=0; col<3; col++) {
        int piv=col;
        for (int r=col+1;r<3;r++) if (fabs(m[r][col])>fabs(m[piv][col])) piv=r;
        if (piv!=col) for (int k=0;k<5;k++){ float t=m[col][k]; m[col][k]=m[piv][k]; m[piv][k]=t; }
        if (fabs(m[col][col])<1e-6f) { Serial.println("[Calib] LOI: 3 diem gan thang hang"); return false; }
        float d=m[col][col];
        for (int k=col;k<5;k++) m[col][k]/=d;
        for (int r=0;r<3;r++){ if (r==col) continue; float f=m[r][col]; for (int k=col;k<5;k++) m[r][k]-=f*m[col][k]; }
    }
    ax=m[0][3]; bx=m[1][3]; dx=m[2][3];
    ay=m[0][4]; by=m[1][4]; dy=m[2][4];
    return true;
}

static bool wantExit() {
    if (!Serial.available()) return false;
    String l = Serial.readStringUntil('\n'); l.trim();
    return l.equalsIgnoreCase("calib:exit");
}

void startTouchCalib() {
    Serial.println("[Calib] Bat dau - cham lan luot 3 diem. Go calib:exit de huy.");
    int W=tft.width(), H=tft.height();
    int ptX[3]={CALIB_MARGIN, W-CALIB_MARGIN, W/2};
    int ptY[3]={CALIB_MARGIN, CALIB_MARGIN,   H-CALIB_MARGIN};
    float rrx[3], rry[3], ssx[3], ssy[3];

    for (int pt=0; pt<3; pt++) {
        tft.fillScreen(TFT_BLACK);
        tft.setTextColor(TFT_WHITE, TFT_BLACK);
        tft.setCursor(10,10); tft.printf("Hieu chuan %d/3", pt+1);
        crosshair(ptX[pt], ptY[pt], 14, TFT_RED);

        while (touchscreen.touched()) delay(10);
        delay(200);

        int16_t rx=0, ry=0; bool got=false;
        uint32_t t0=millis();
        while (millis()-t0 < 15000) {
            if (wantExit()) { Serial.println("[Calib] Da huy"); setUIMode(MODE_FACE); return; }
            if (touchscreen.touched()) { delay(100); got=readAvgRaw(rx,ry); if (got) break; }
            delay(30);
        }
        if (!got) { Serial.println("[Calib] LOI: timeout"); setUIMode(MODE_FACE); return; }

        crosshair(ptX[pt], ptY[pt], 14, TFT_GREEN);
        rrx[pt]=rx; rry[pt]=ry; ssx[pt]=ptX[pt]; ssy[pt]=ptY[pt];
        Serial.printf("[Calib] Diem %d: man(%d,%d) <- raw(%d,%d)\n", pt+1, ptX[pt], ptY[pt], rx, ry);
        delay(400);
    }

    if (!solveAffine(rrx,rry,ssx,ssy, _c.ax,_c.bx,_c.dx, _c.ay,_c.by,_c.dy)) {
        Serial.println("[Calib] That bai, thu lai (diem gan thang hang)");
        setUIMode(MODE_FACE); return;
    }
    _c.valid = true;
    Serial.printf("[Calib] He so: ax=%.6f bx=%.6f dx=%.4f | ay=%.6f by=%.6f dy=%.4f\n",
                  _c.ax,_c.bx,_c.dx, _c.ay,_c.by,_c.dy);

    int vx=W/2, vy=H*2/3;
    tft.fillScreen(TFT_BLACK);
    tft.setCursor(10,10); tft.print("Xac nhan: cham dau + duoi day");
    crosshair(vx, vy, 12, TFT_CYAN);
    while (touchscreen.touched()) delay(10);
    delay(200);

    int16_t vrx=0, vry=0; bool vgot=false;
    uint32_t tv=millis();
    while (millis()-tv < 10000) {
        if (touchscreen.touched()) { delay(100); vgot=readAvgRaw(vrx,vry,16,2000); if (vgot) break; }
        delay(30);
    }
    if (vgot) {
        int cx=(int)(_c.ax*vrx + _c.bx*vry + _c.dx);
        int cy=(int)(_c.ay*vrx + _c.by*vry + _c.dy);
        int err=(int)sqrt((float)((cx-vx)*(cx-vx)+(cy-vy)*(cy-vy)));
        Serial.printf("[Calib] Sai so xac nhan: %d px (target %d,%d - do %d,%d)\n", err, vx, vy, cx, cy);
    } else {
        Serial.println("[Calib] Timeout xac nhan - van luu he so vua do");
    }

    _prefs.begin("touchcal", false);
    _prefs.putFloat("ax",_c.ax); _prefs.putFloat("bx",_c.bx); _prefs.putFloat("dx",_c.dx);
    _prefs.putFloat("ay",_c.ay); _prefs.putFloat("by",_c.by); _prefs.putFloat("dy",_c.dy);
    _prefs.putBool("ok", true);
    _prefs.end();
    Serial.println("[Calib] Da luu NVS (namespace touchcal)");

    setUIMode(MODE_FACE);
}