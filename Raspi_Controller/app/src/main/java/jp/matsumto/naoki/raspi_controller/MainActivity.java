package jp.matsumto.naoki.raspi_controller;

import android.app.Activity;
import android.hardware.Sensor;
import android.hardware.SensorEvent;
import android.hardware.SensorEventListener;
import android.hardware.SensorManager;
import android.os.Bundle;
import android.os.Handler;
import android.view.View;
import android.widget.Button;
import android.widget.SeekBar;
import android.widget.TextView;

import java.io.IOException;
import java.io.PrintWriter;
import java.net.ConnectException;
import java.net.Socket;
import java.util.Timer;
import java.util.TimerTask;


public class MainActivity extends Activity {
    private static final int MATRIX_SIZE = 16;
    private float[] inR = new float[MATRIX_SIZE];
    private float[] outR = new float[MATRIX_SIZE];
    private float[] I = new float[MATRIX_SIZE];
    private int angle;
    private TextView textView1;
    private Button btnSt;
    private Handler handler = new Handler();
    private SeekBar seekbar ;
    private boolean running = true;
    private int speed;
    private Timer tcpTimer;
    int ConnectionRefresh = 0;
    private Runnable runnable = new Runnable() {
        public void run() {
            if (running) {
                speed = seekbar.getProgress();
                handler.postDelayed(runnable, 50);
                String buf =
                        "---------- Orientation --------\n" +
                                String.format("速度\n\t%d\n", speed) +
                                String.format("前後の傾斜\n\t%d\n", angle);
                textView1.setText(buf);
            }
        }
    };
    private SensorManager manager;
    private SensorEventListener listener;
    private float[] fAccell;
    private float[] fMagnetic;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);
        textView1 = findViewById(R.id.text1);
        seekbar = findViewById(R.id.seekBar);
        btnSt = findViewById(R.id.btnSt);
        final Handler UIhandler = new Handler();
        manager = (SensorManager) getSystemService(SENSOR_SERVICE);
        listener = new SensorEventListener() {
            // 値変更時
            @Override
            public void onSensorChanged(SensorEvent sensorEvent) {
                switch (sensorEvent.sensor.getType()) {
                    case Sensor.TYPE_ACCELEROMETER:
                        fAccell = sensorEvent.values.clone();
                        break;
                    case Sensor.TYPE_MAGNETIC_FIELD:
                        fMagnetic = sensorEvent.values.clone();
                        break;
                }
                if (fAccell != null && fMagnetic != null) {
                    SensorManager.getRotationMatrix(inR, I, fAccell, fMagnetic);
                    SensorManager.remapCoordinateSystem(inR, SensorManager.AXIS_X, SensorManager.AXIS_Y, outR);
                    float[] fAttitude = new float[3];
                    SensorManager.getOrientation(outR, fAttitude);
                    fAttitude[1] = radianToDegrees(fAttitude[1]);
                    angle = (int) fAttitude[1];
                }
            }
            @Override
            public void onAccuracyChanged(Sensor sensor, int i) {

            }
        };
        btnSt.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                runnable.run();
                tcpTimer = new Timer(true);
                tcpTimer.schedule(new TimerTask() {
                    static final String address = "192.168.10.1";
                    static final int port = 10000;
                    Boolean initialized = false;
                    boolean serverOnline = true;
                    PrintWriter pw;
                    Socket socket;

                    @Override
                    public void run() {
                        if (!initialized || ConnectionRefresh > 20) {
                            try {
                                if(socket != null){
                                    socket.close();
                                    socket = null;
                                }
                                ConnectionRefresh = 0;
                                socket = new Socket(address, port);
                                initialized = true;
                                pw = new PrintWriter(socket.getOutputStream(), true);
                                UIhandler.post(new Runnable() {
                                    @Override
                                    public void run() {
                                        btnSt.setText("Connected!");
                                    }
                                });
                            } catch (ConnectException e) {
                                serverOnline = false;
                                e.printStackTrace();
                                UIhandler.post(new Runnable() {
                                    @Override
                                    public void run() {
                                        btnSt.setText("Disconnected!");
                                    }
                                });
                            } catch (IOException e) {
                                e.printStackTrace();
                            }
                        }
                        if (serverOnline) {
                            pw.printf("ANGLE %d SPEED %d", angle, speed);
                            ConnectionRefresh++;
                        }
                    }
                }, 100, 50);
            }
        });
    }

    @Override
    protected void onResume() {
        super.onResume();
        running = true;
        manager.registerListener(
                listener,
                manager.getDefaultSensor(Sensor.TYPE_ACCELEROMETER),
                SensorManager.SENSOR_DELAY_FASTEST);
        manager.registerListener(
                listener,
                manager.getDefaultSensor(Sensor.TYPE_MAGNETIC_FIELD),
                SensorManager.SENSOR_DELAY_FASTEST);
    }


    @Override
    protected void onPause() {
        super.onPause();
        running = false;
        if(tcpTimer != null) {
            tcpTimer.cancel();
        }
        manager.unregisterListener(listener);
    }

    private int radianToDegrees(float angrad) {
        return (int) Math.floor(angrad >= 0 ? Math.toDegrees(angrad) : 360 + Math.toDegrees(angrad));
    }


}