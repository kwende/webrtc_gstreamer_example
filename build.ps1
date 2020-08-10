cd Docker
docker build -t brush/gstreamer_android .
docker run -dt --rm --name gstreamer_android -v ${PWD}:/android brush/gstreamer_android
docker exec gstreamer_android /bin/bash -c "cd /android; gradle assembleDebug;"
#D:\android\android-sdk\platform-tools\adb.exe push ${PWD}\app\build\outputs\apk\debug\app-debug.apk /sdcard/Download/app-debug.apk
#docker-compose up
cd ..