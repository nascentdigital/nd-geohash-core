echo "Downloading NodeJS 6"
curl -sL https://deb.nodesource.com/setup_6.x | sudo -E bash - \
    && echo "Installing NodeJS 6" \
    && sudo apt-get install -y nodejs \
    && echo "Installed NodeJS Addon tools" \
    && sudo apt-get install -y build-essential \
    && echo "Completed NodeJS installation"
