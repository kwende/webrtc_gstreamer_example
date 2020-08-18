cd docker
docker build -t brush/gstreamer_android .
updatecd ..
docker run -dt --rm --name gstreamer_android -v ${PWD}:/android brush/gstreamer_android
docker exec gstreamer_android /bin/bash -c "cd /android; gradle assembleDebug;"
.\platform-tools\adb.exe install ${PWD}\app\build\outputs\apk\debug\app-debug.apk
docker-compose up