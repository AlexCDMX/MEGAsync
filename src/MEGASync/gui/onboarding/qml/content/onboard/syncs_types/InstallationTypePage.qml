InstallationTypePageForm {

    footerButtons {

        rightSecondary.onClicked: {
            syncsFlow.state = computerName;
        }

        rightPrimary.onClicked: {
            switch(buttonGroup.checkedButton.type) {
                case SyncsType.Sync:
                    syncsFlow.state = syncs;
                    break;
                case SyncsType.Backup:
                    syncsFlow.state = backupsFlow;
                    break;
                default:
                    console.error("Button type does not exist -> "
                                  + buttonGroup.checkedButton.type);
                    break;
            }
        }
    }

    buttonGroup.onCheckStateChanged: {
        if(buttonGroup.checkedButton != null) {
            footerButtons.rightPrimary.enabled = true;
        }
    }

}
