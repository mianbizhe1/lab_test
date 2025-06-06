import {baseURL, GET, POST} from '@/utils/request'
import CoreConsts from "@/components/CoreConsts";

// 图像验证码url
const captchaUri = `${baseURL}/valid/captcha`
const avatarUploadUri = "/core/center/avatar"

export {captchaUri, avatarUploadUri}

/**
 * 获取校验码需要的code
 */
export function getCode() {
    return GET("/valid/code");
}

// 获取侧边菜单栏数据
export function getMenus() {
    return GET("/core/menu/bars")
}

// 获取用户详情
export function getUser() {
    return GET('/core/admin/detail')
}

// 获取权限
export function getPermissions() {
    return GET('/core/menu/permissions')
}

// 系统配置信息
export function getSysConfig(config) {
    return GET('/core/config/list', config || {})
}

// 用户中心 - 修改用户密码
export function editPwd(model) {
    return POST("/core/center/pwd", model)
}

// 用户中心 - 修改当前用户信息
export function editUser(user) {
    return POST('/core/center/editUser', user);
}

export function getNotifyList() {
    return GET('/core/notify/view')
}

// 获取字典数据
export function getDict(type) {
    return GET('/core/dictData/listByType', {type}).then(({data}) => {
        let options;
        if(data instanceof Array) {
            options = data.map(item => {return {label: item.label, value: item.value}})
        }

        return options;
    })
}

// 提交登录接口
export function login(user) {
    let config = {};
    config[CoreConsts.CancelRespResolver] = true;
    return POST(`/core/login?code=${user.code}`, user, config);
}

// 注销系统
export function logout() {
    return POST('/core/logout')
}

// oauth2授权认证
export function oauth2(type) {
    return GET("/core/oauth2", {params: {type}}).then(resp => {
        location.href = resp.data
    })
}
