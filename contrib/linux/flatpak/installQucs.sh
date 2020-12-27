flatpak-builder --repo=QucsRepo --force-clean Qucs net.sourceforge.qucs.json
flatpak remote-add QucsRepo QucsRepo --no-gpg-verify
flatpak install QucsRepo net.sourceforge.qucs
