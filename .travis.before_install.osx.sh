# Download and extract FBX SDK installer
wget http://download.autodesk.com/us/fbx_release_older/2016.1.2/fbx20161_2_fbxsdk_clang_mac.pkg.tgz
tar xvzf fbx20161_2_fbxsdk_clang_mac.pkg.tgz
rm fbx20161_2_fbxsdk_clang_mac.pkg.tgz

# Run the installer pkg
sudo installer -allowUntrusted -verboseR -pkg ./fbx20161_2_fbxsdk_clang_macos.pkg -target /
rm -r fbx20161_2_fbxsdk_clang_macos.pkg

# Create symlink at expected location
ln -s "/Applications/Autodesk/FBX SDK/2016.1.2/" $FBX_SDK
