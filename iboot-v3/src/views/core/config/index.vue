<template>
  <UView name="配置">
    <UViewSearch>
      <URow col="search">
        <UInputItem field="name" label="配置名称"/>
        <UInputItem field="label" label="配置标识"/>
        <URadioItem field="type" label="系统配置" :options="type"/>
        <UFuncBtn func="query" url="/core/config/view">搜索</UFuncBtn>
        <UFuncBtn func="reset">重置</UFuncBtn>
        <UFuncBtn func="add" url="/core/config/add" v-auth="'core:config:add'">新增</UFuncBtn>
      </URow>
    </UViewSearch>
    <UViewTable :columns="columns" :scroll="{x: 1000}">
      <template #c_action="{record}">
        <UFuncTag func="edit" :data="record" url="/core/config/edit" v-auth="'core:config:edit'">修改</UFuncTag>
        <UFuncTag func="del" :data="record" url="/core/config/del" v-auth="'core:config:del'">删除</UFuncTag>
      </template>
    </UViewTable>
    <UViewModal title="配置管理" :span="[6, 15]" :rules="rules">
      <template #default="{model}">
        <UInputItem field="name" label="配置名称"/>
        <UInputItem field="label" label="配置标识" :disabled="model.id != null"/>
        <UInputItem field="value" label="配置值" />
        <URadioItem field="type" label="系统配置" :options="type" defaultValue="def"/>
        <UTextareaItem field="remark" label="配置说明" />
      </template>
      <template #footer="{model}">
        <UFuncBtn func="cancel">取消</UFuncBtn>
        <UFuncBtn func="submit" :url="model.id ? '/core/config/edit' : '/core/config/add'" :callback="reloadConfig">提交</UFuncBtn>
        <UFuncBtn func="reset">重置</UFuncBtn>
      </template>
    </UViewModal>
  </UView>
</template>
<!--系统配置管理-->
<script>

import CoreConsts from "@/components/CoreConsts";
import {useStore} from "vuex";

export default {
  name: "Config",
  setup() {
    let type = [
      {label: '是', value:'sys'}, {label: '否', value: 'def'}
    ]
    let columns = [
      {field: 'name', title: '配置名称'},
      {field: 'label', title: '配置标识'},
      {field: 'value', title: '配置值'},
      {field: 'type', title: '系统配置', options: type},
      {field: 'remark', title: '配置说明', width: 360},
      {field: 'action', type: 'action', title: '操作'}
    ]
    let rules = {
      name: {required: true, message: '配置名称必填'},
      label: {required: true, message: '配置标识必填'},
      value: {required: true, message: '配置值必填'},
    }

    let store = useStore();
    let reloadConfig = ({code}) => {
      if(code == CoreConsts.SuccessCode) {
        // 重新初始化菜单
        store.dispatch('sys/initConfig');
      }

      return Promise.resolve();
    }
    return {columns, type, rules, reloadConfig}
  }
}
</script>

<style scoped>

</style>
