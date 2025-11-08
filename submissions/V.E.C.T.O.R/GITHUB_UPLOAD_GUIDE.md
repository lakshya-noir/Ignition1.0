# How to Upload Project to GitHub

## Step 0: Install Git (If Not Already Installed)

**If you see "git is not recognized" error, you need to install Git first:**

1. **Download Git for Windows:**
   - Go to: https://git-scm.com/download/win
   - The download will start automatically
   - Or use direct link: https://github.com/git-for-windows/git/releases/latest

2. **Install Git:**
   - Run the downloaded installer (e.g., `Git-2.xx.x-64-bit.exe`)
   - Click "Next" through the installation
   - **Important**: Keep default options selected
   - When asked about "Choosing the default editor", you can choose "Nano" or "Notepad"
   - Click "Install"
   - Wait for installation to complete

3. **Verify Installation:**
   - Close and reopen Command Prompt/PowerShell
   - Run: `git --version`
   - You should see something like: `git version 2.xx.x`

4. **Configure Git (First Time Setup):**
   ```bash
   git config --global user.name "Your Name"
   git config --global user.email "your.email@example.com"
   ```

**Now proceed to Step 1 below.**

---

## Step 1: Create a GitHub Repository

1. Go to https://github.com
2. Click the **"+"** icon in the top right → **"New repository"**
3. Fill in:
   - **Repository name**: `rider-telemetry-mobile` (or your preferred name)
   - **Description**: "Rider Telemetry System - Mobile app with ESP32 sensor integration"
   - **Visibility**: Choose Public or Private
   - **DO NOT** check "Initialize with README" (we already have files)
4. Click **"Create repository"**

## Step 2: Initialize Git in Your Project (if not already done)

Open Command Prompt or PowerShell in your project folder:

```bash
cd C:\Users\HOME\rider-telemetry-mobile
```

Check if Git is already initialized:
```bash
git status
```

If you see "not a git repository", initialize it:
```bash
git init
```

## Step 3: Add All Files to Git

```bash
# Add all files
git add .

# Check what will be committed
git status
```

## Step 4: Create Initial Commit

```bash
git commit -m "Initial commit: Rider Telemetry Mobile App with ESP32 integration"
```

## Step 5: Connect to GitHub Repository

After creating the repository on GitHub, you'll see a page with instructions. Use these commands:

```bash
# Add the remote repository (replace YOUR_USERNAME with your GitHub username)
git remote add origin https://github.com/YOUR_USERNAME/rider-telemetry-mobile.git

# Verify the remote was added
git remote -v
```

## Step 6: Push to GitHub

```bash
# Push to main branch (or master if that's your default)
git branch -M main
git push -u origin main
```

If prompted for credentials:
- **Username**: Your GitHub username
- **Password**: Use a **Personal Access Token** (not your GitHub password)
  - Generate one at: https://github.com/settings/tokens
  - Select scope: `repo` (full control of private repositories)

## Step 7: Verify Upload

1. Go to your GitHub repository page
2. You should see all your files including:
   - `App.js`
   - `src/` folder
   - `esp32/` folder with the Arduino code
   - `package.json`
   - All other project files

## Optional: Add .gitignore Updates

Your `.gitignore` already excludes:
- `node_modules/`
- `.expo/`
- Build files

This is good - it prevents uploading unnecessary files.

## Future Updates

When you make changes and want to upload them:

```bash
# Check what changed
git status

# Add changed files
git add .

# Commit changes
git commit -m "Description of your changes"

# Push to GitHub
git push
```

## Repository Structure

Your GitHub repo will have this structure:

```
rider-telemetry-mobile/
├── esp32/
│   ├── esp32_mpu6050_telemetry.ino
│   └── README.md
├── src/
│   ├── context/
│   ├── hooks/
│   ├── screens/
│   └── utils/
├── App.js
├── package.json
├── app.json
├── gps_server.js
├── README.md
└── ... (other files)
```

## Troubleshooting

**If you get "remote origin already exists":**
```bash
git remote remove origin
git remote add origin https://github.com/YOUR_USERNAME/rider-telemetry-mobile.git
```

**If you get authentication errors:**
- Use Personal Access Token instead of password
- Or use SSH: `git@github.com:YOUR_USERNAME/rider-telemetry-mobile.git`

**If you want to update existing repository:**
```bash
git pull origin main
git add .
git commit -m "Your commit message"
git push origin main
```

