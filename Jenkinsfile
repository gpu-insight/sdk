import hudson.FilePath
import jenkins.model.Jenkins

def artifacts = [
    sdk: [
        rpm: '',
        version: ''
    ]
]

def githash=""
def releases=['debug']
//def archs = ['x86_64', 'mips', 'aarch64']
def archs = ['x86_64']
def distribution = 'el7.centos'
def latestSdkRpm=""

def properties2String(prop) {
    def sw = new StringWriter()
    prop.store(sw, null)
    return sw.toString()
}

def artifact(build, name) {
   def rawBuild = build.getRawBuild();

    def previousArtifacts = rawBuild.getArtifactManager()
    return previousArtifacts.root().child(name)
}

pipeline {
    agent {
        node {
            label 'sdk'
        }
    }
    triggers {
        pollSCM('H/2 * * * *')
    }
    options {
        disableConcurrentBuilds();
    }
    environment {
        YUM_CONF_HOME='/home/jenkins/yum.d'
        RPMHOME='/jenkins/rpmbuild'
        RPM_LOCAL_REPO_DIR='/jenkins/botson_repos'
        RPM_REMOTE_REPO_ADDR='192.168.50.83'
        RPM_REMOTE_REPO_DIR='/var/ftp/pub/iso/repos/botson'
    }
    stages {
        stage('setup') {
            steps {
                // testing rsync
                sh script: "mkdir -p ${RPM_LOCAL_REPO_DIR} || test 0"
                withCredentials([usernamePassword(credentialsId: '173377c3-b959-4ba7-bd79-694d79f61733', passwordVariable: 'RPM_REMOTE_REPO_PASSWORD', usernameVariable: 'RPM_REMOTE_REPO_USERNAME')]) {
                    sh script: "sshpass -p \"${RPM_REMOTE_REPO_PASSWORD}\" rsync -ravz --delete -e \"ssh -o UserKnownHostsFile=/dev/null -o StrictHostKeyChecking=no\"  ${RPM_REMOTE_REPO_USERNAME}@${RPM_REMOTE_REPO_ADDR}:${RPM_REMOTE_REPO_DIR}/ ${RPM_LOCAL_REPO_DIR}/"
                }
            }
        }

        stage('build') {
            steps {
                checkout([$class: 'GitSCM',
                    branches: [[name: '*/master']],
                    doGenerateSubmoduleConfigurations: false,
                    extensions: [[$class: 'SubmoduleOption',
                        disableSubmodules: false,
                        parentCredentials: true,
                        recursiveSubmodules: true,
                        reference: '',
                        trackingSubmodules: false
                    ]],
                    submoduleCfg: [],
                    userRemoteConfigs: [[credentialsId: 'd506c69d-6153-4a2f-b3e2-3daf87e76522', url: 'http://gitlab.botson.com/bts_repos/gb/udriver/btx_sdk.git']]
                ])
                sh script: 'pwd'
                sh script: 'git clean -fdx'
                script {
                    githash = sh(returnStdout:true, script: "git log -n 1 --pretty=format:'%h'").trim()
                    artifacts['sdk']['version'] = "${currentBuild.number}.${githash}"
                    sh script: "cp --remove-destination sdk.spec ${RPMHOME}/SPECS/botson-sdk.spec"
                }

                dir("..") {
                    sh script: "tar -czf botson-sdk-${artifacts['sdk']['version']}.tar.gz --exclude .git --transform s/^sdk/botson-sdk-${artifacts['sdk']['version']}/ sdk"
                    sh script: "mv botson-sdk-${artifacts['sdk']['version']}.tar.gz ${RPMHOME}/SOURCES"
                }
            }
        }
        stage('package') {
            steps {
                script {
                    archs.each { arch ->
                        releases.each { release ->
                            dir("${RPMHOME}/SPECS") {
                                sh script: "rpmbuild --bb --rmsource --rmspec --clean --target=${arch} -D '_topdir ${RPMHOME}' -D 'sdk_release ${release}' -D 'sdk_version ${artifacts['sdk']['version']}' botson-sdk.spec"
                            }
                            dir("${RPMHOME}/RPMS/${arch}") {
                                // to rsync
                                sh script: "cp --remove-destination botson-sdk-${artifacts['sdk']['version']}-${release}.${distribution}.${arch}.rpm ${RPM_LOCAL_REPO_DIR}"
                                // to artifact
                                sh script: "cp --remove-destination botson-sdk-${artifacts['sdk']['version']}-${release}.${distribution}.${arch}.rpm ${WORKSPACE}"
                            }
                            script {
                                latestSdkRpm = "botson-sdk-${artifacts['sdk']['version']}-${release}.${distribution}.${arch}.rpm"
                            }
                        }
                    }
                }
            }
        }
        stage('publish') {
            steps  {
            }
        }
    }

    post {
        success {
            script {
                def sdkProperties = new Properties()
                def arti = artifact(currentBuild, "sdk.build.properties")
                if (arti.exists()) {
                    sdkProperties.load(arti.open())
                }
                sdkProperties["sdk.version"] = artifacts['sdk']['version'].toString()
                sdkProperties["sdk.rpm"] = "botson-sdk-${artifacts['sdk']['version']}-debug.el7.centos.x86_64.rpm".toString()
                writeFile encoding: 'utf-8', file: 'sdk.build.properties', text: properties2String(sdkProperties)
                archiveArtifacts artifacts: 'sdk.build.properties', fingerprint: true
                archiveArtifacts artifacts: latestSdkRpm, fingerprint: true
            }

            echo 'publish'
            withCredentials([usernamePassword(credentialsId: '173377c3-b959-4ba7-bd79-694d79f61733', passwordVariable: 'RPM_REMOTE_REPO_PASSWORD', usernameVariable: 'RPM_REMOTE_REPO_USERNAME')]) {
                sh script: "sshpass -p \"${RPM_REMOTE_REPO_PASSWORD}\" rsync -ravz -e \"ssh -o UserKnownHostsFile=/dev/null -o StrictHostKeyChecking=no\"  ${RPM_REMOTE_REPO_USERNAME}@${RPM_REMOTE_REPO_ADDR}:${RPM_REMOTE_REPO_DIR}/ ${RPM_LOCAL_REPO_DIR}/"
                sh script: "createrepo --update ${RPM_LOCAL_REPO_DIR}"
                sh script: "sshpass -p \"${RPM_REMOTE_REPO_PASSWORD}\" rsync -ravz -e \"ssh -o UserKnownHostsFile=/dev/null -o StrictHostKeyChecking=no\" ${RPM_LOCAL_REPO_DIR}/* ${RPM_REMOTE_REPO_USERNAME}@${RPM_REMOTE_REPO_ADDR}:${RPM_REMOTE_REPO_DIR}"
            }
        }

        cleanup {
            withCredentials([usernamePassword(credentialsId: '1000', passwordVariable: 'JENKINS_PASSWORD', usernameVariable: 'JENKINS_USERNAME')]) {
                script {
                    dir("${WORKSPACE}") {
                        sh script: "pwd"
                        sh script: "sshpass -p \"${JENKINS_PASSWORD}\" sudo -u root git clean -fdx"
                    }

                    dir("${RPMHOME}/BUILD") {
                        sh script: "find . -maxdepth 1 -name 'botson-sdk*' -type d ! -path . | sort -nk3 -t- | sed '\$d' | xargs rm -rf"
                    }

                    dir("${RPMHOME}/BUILDROOT") {
                        sh script: "find . -maxdepth 1 -name 'botson-sdk*' -type d ! -path . | sort -nk3 -t- | sed '\$d' | xargs rm -rf"
                    }
		    
                    dir("${RPMHOME}/RPMS") {
                        sh script: "find . -name 'botson-sdk*.rpm' -type f ! -path . | xargs rm -rf"
                    }
                }
            }
        }
    }
}
