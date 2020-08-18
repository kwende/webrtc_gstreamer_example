cd Docker
docker build -t brush/gstreamer_android .
cd ..
docker run -dt --rm --name gstreamer_android -v ${PWD}:/android brush/gstreamer_android
docker exec gstreamer_android /bin/bash -c "cd /android; gradle clean assembleDebug;"
Start-Sleep 2
cp .\app\build\intermediates\transforms\stripDebugSymbol\debug\0\lib\armeabi-v7a\*.so .\Xamarin\Xamarin.Android\Libraries\armeabi-7a
cp .\app\build\intermediates\transforms\stripDebugSymbol\debug\0\lib\x86\*.so .\Xamarin\Xamarin.Android\Libraries\android-x86
docker-compose up