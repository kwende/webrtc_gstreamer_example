#this is being done because I couldn't figure out how to get all of this 
#functionality to work in a docker compose file. probably just me not understanding something. 

# build the gstreamer dev environment, including copying over the patch files. 
# cd GStreamer_Build
# docker build -t brush/gstreamer-1.16-build-environment .
# cd ..

# # build the android dev environment
# cd Android_Build
# docker build -t brush/android-build-environment .
# cd ..

# create a volume to hold the binaries
#docker volume create gstreamer_android_binaries

# spin up the gstreamer build environment to copy over build files
#docker run -d --rm -v gstreamer_android_binaries:/output --name gstreamer-1.16-build-environment brush/gstreamer-1.16-build-environment
#docker exec gstreamer-1.16-build-environment bash -c "cp -R /cerbero/build/dist/android_universal/. /output"

# spin up the android development environment, mount the build files as /usr/local/gstreamer
#docker run -d --rm -v gstreamer_android_binaries:/usr/local/gstreamer -v ${PWD}/Output:/output -v ${PWD}/../..:/android --name android-build-environment brush/android-build-environment
docker exec android-build-environment bash -c "cd /android && gradle clean assembleDebug && cp -R /android/app/build/intermediates/transforms/stripDebugSymbol/debug/0/lib/. /output && cp -R /android/app/build/outputs/apk/debug/app-debug.apk /output"

# clean up
#docker stop gstreamer-1.16-build-environment
#docker stop android-build-environment
#docker volume rm gstreamer_android_binaries


#Start-Sleep 2
#cp ..\..\app\build\intermediates\transforms\stripDebugSymbol\debug\0\lib\armeabi-v7a\*.so ..\..\Xamarin\Xamarin.Android\Libraries\armeabi-7a
#cp ..\..\app\build\intermediates\transforms\stripDebugSymbol\debug\0\lib\x86\*.so ..\..\Xamarin\Xamarin.Android\Libraries\android-x86
#docker-compose up