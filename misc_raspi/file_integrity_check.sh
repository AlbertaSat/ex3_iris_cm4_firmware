#!/bin/bash
set +x
BASE_DIR=$(pwd)

#ADD ERROR HANDLING - CURRENTLY IM ASSUMING THAT ALL FUNCTIONS WORK
#ADD WHAT TO DO IF ALL FILES ARE CORRUPTED

# Configuration Files / Folders
PRIMARY_FOLDER="$BASE_DIR/deploy_primary"
SECONDARY_FOLDER="$BASE_DIR/deploy_secondary"
TERTIARY_FOLDER="$BASE_DIR/deploy_tertiary"
CHECKSUM_FILE="checksum.txt"

MAIN_EXEC="main"

LOG_FILE="/var/log/folder_integrity.log"

# Function to compute the File checksum
compute_checksum() {
    #echo "Computing checksum for: $1/$MAIN_EXEC $1/$CHECKSUM_FILE"
    sha256sum "$1/$MAIN_EXEC" > "$1/$CHECKSUM_FILE"
}

# Function to validate File using checksum
validate_checksum() {
    if sha256sum -c "$1/$CHECKSUM_FILE"; then
        echo "Checksum verification passed."
        return 0  # Indicate success
    else
        echo "Checksum verification failed!" >&2
        return 1  # Indicate failure
    fi
}

# Function to Replace Corrupted File with 'GOOD' File
replace_file(){

    echo $1
    echo $2
#                                                    GOODFILE          TEMPFILE
    cp --reflink=auto --preserve=mode,timestamps "$2/$MAIN_EXEC" "$1/$MAIN_EXEC.tmp"

    mv -f "$1/$MAIN_EXEC.tmp" "$1/$MAIN_EXEC"
    compute_checksum "$1"
}

# Read the stored checksum
# if [ ! -f "$CHECKSUM_FILE" ]; then
#     echo "Error: Reference checksum file not found!" | tee -a "$LOG_FILE"
#     exit 1
# fi

# Check File Integrities
validate_checksum "$PRIMARY_FOLDER"
PRIMARY_VALIDATE=$?

validate_checksum "$SECONDARY_FOLDER"
SECONDARY_VALIDATE=$?

validate_checksum "$TERTIARY_FOLDER"
TERTIARY_VALIDATE=$?

#Go through File Integrity Results, than repair them if it failed
if [ $PRIMARY_VALIDATE -eq 1 ]; then
    echo "Proceeding to fix Primary."

    #Finds 'Good' File to Copy
    if [ $SECONDARY_VALIDATE -eq 0 ]; then
        echo "Using Secondary"
        replace_file "$PRIMARY_FOLDER" "$SECONDARY_FOLDER"
    elif [ $TERTIARY_VALIDATE -eq 0 ]; then
        echo "Using Tertiary"
        replace_file "$PRIMARY_FOLDER" "$TERTIARY_FOLDER"
    fi
fi


if [ $SECONDARY_VALIDATE -eq 1 ]; then
    echo "Proceeding to fix Secondary."

    #Finds 'Good' File to Copy
    if [ $PRIMARY_VALIDATE -eq 0 ]; then
        echo "Using Primary"
        replace_file "$SECONDARY_FOLDER" "$PRIMARY_FOLDER"
    elif [ $TERTIARY_VALIDATE -eq 0 ]; then
        echo "Using Tertiary"
        replace_file "$SECONDARY_FOLDER" "$TERTIARY_FOLDER"
    fi
fi


if [ $TERTIARY_VALIDATE -eq 1 ]; then
    echo "Proceeding to fix Tertiary."

    #Finds 'Good' File to Copy
    if [ $PRIMARY_VALIDATE -eq 0 ]; then
        echo "Using Primary"
        replace_file "$TERTIARY_FOLDER" "$PRIMARY_FOLDER"
    elif [ $SECONDARY_VALIDATE -eq 0 ]; then
        echo "Using Secondary"
        replace_file "$TERTIARY_FOLDER" "$PRIMARY_FOLDER"
    fi
fi



# # Check integrity
# if [ "$CURRENT_CHECKSUM" == "$EXPECTED_CHECKSUM" ]; then
#     echo "$(date) - Integrity check passed. No action needed." | tee -a "$LOG_FILE"
# else
#     echo "$(date) - Integrity check failed! Restoring from backup..." | tee -a "$LOG_FILE"
    
#     # Restore folder from backup
#     rm -rf "$FOLDER_TO_CHECK"
#     cp -r "$BACKUP_FOLDER" "$FOLDER_TO_CHECK"

#     # Recompute checksum after restore
#     NEW_CHECKSUM=$(compute_checksum "$FOLDER_TO_CHECK")

#     if [ "$NEW_CHECKSUM" == "$EXPECTED_CHECKSUM" ]; then
#         echo "$(date) - Restore successful. Folder integrity restored." | tee -a "$LOG_FILE"
#     else
#         echo "$(date) - Restore failed! Check manually." | tee -a "$LOG_FILE"
#     fi
# fi