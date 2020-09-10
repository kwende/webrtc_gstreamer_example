FROM ubuntu:latest
USER root

# Install Build Essentials
# Install OpenJDK-8
# Fix certificate issues
ENV DEBIAN_FRONTEND=noninteractive

RUN apt-get update && \
	apt-get install unzip && \
	apt-get install build-essential -y && \
	apt-get install wget -y && \
	apt-get install file -y && \
	apt-get install apt-utils -y && \
	apt-get install vim -y && \
	apt-get install pkg-config -y && \
	apt-get install curl -y && \
	apt-get install -y openjdk-8-jdk && \
    apt-get install -y ant && \
	apt-get install ca-certificates-java && \
	apt-get install git -y && \
    apt-get clean && \
	update-ca-certificates -f;

# Setup JAVA_HOME -- useful for docker commandline
ENV JAVA_HOME /usr/lib/jvm/java-8-openjdk-amd64/
RUN export JAVA_HOME

ENV GRADLE_VERSION 4.6
ENV GRADLE_SDK_URL https://services.gradle.org/distributions/gradle-${GRADLE_VERSION}-bin.zip
RUN mkdir /usr/local/gradle
RUN curl -sSL "${GRADLE_SDK_URL}" -o gradle-${GRADLE_VERSION}-bin.zip  \
	&& unzip gradle-${GRADLE_VERSION}-bin.zip -d /usr/local/gradle  \
	&& rm -rf gradle-${GRADLE_VERSION}-bin.zip
ENV GRADLE_HOME /usr/local/gradle/gradle-${GRADLE_VERSION}
ENV PATH ${GRADLE_HOME}/bin:$PATH

ENV SDK_URL="https://dl.google.com/android/repository/sdk-tools-linux-3859397.zip" \
    ANDROID_HOME="/usr/local/android-sdk" \
    ANDROID_VERSION=28 \
    ANDROID_BUILD_TOOLS_VERSION=27.0.3
# Download Android SDK
RUN mkdir "$ANDROID_HOME" .android \
    && cd "$ANDROID_HOME" \
    && curl -o sdk.zip $SDK_URL \
    && unzip sdk.zip \
    && rm sdk.zip \
    && mkdir "$ANDROID_HOME/licenses" || true \
    && echo "24333f8a63b6825ea9c5514f83c2829b004d1fee" > "$ANDROID_HOME/licenses/android-sdk-license"
#    && yes | $ANDROID_HOME/tools/bin/sdkmanager --licenses
# Install Android Build Tool and Libraries
RUN $ANDROID_HOME/tools/bin/sdkmanager --update
RUN $ANDROID_HOME/tools/bin/sdkmanager "build-tools;${ANDROID_BUILD_TOOLS_VERSION}" \
    "platforms;android-${ANDROID_VERSION}" \
    "platform-tools"

# Android NDK
ENV ANDROID_NDK_VERSION r21b
ENV ANDROID_NDK_URL http://dl.google.com/android/repository/android-ndk-${ANDROID_NDK_VERSION}-linux-x86_64.zip
RUN curl -L "${ANDROID_NDK_URL}" -o android-ndk-${ANDROID_NDK_VERSION}-linux-x86_64.zip
RUN unzip android-ndk-${ANDROID_NDK_VERSION}-linux-x86_64.zip -d /usr/local/android-ndk-${ANDROID_NDK_VERSION}
RUN rm -rf android-ndk-${ANDROID_NDK_VERSION}-linux-x86_64.zip
ENV ANDROID_NDK_HOME /usr/local/android-ndk-${ANDROID_NDK_VERSION}
ENV PATH ${ANDROID_NDK_HOME}:$PATH
#RUN chmod u+x ${ANDROID_NDK_HOME}/ -R

RUN mkdir /usr/local/gstreamer
ENV GSTREAMER_ROOT_ANDROID /usr/local/gstreamer/
ENV PATH ${GSTREAMER_ROOT_ANDROID}:$PATH
ENV PATH /usr/local/gstreamer/armv7/include/gstreamer-1.0:$PATH

WORKDIR /android

CMD tail -f /dev/null
