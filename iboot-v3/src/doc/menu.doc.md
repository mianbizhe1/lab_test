<h2>IvzMenuView视图组件 -- 一款可以通过菜单生成功能点的通用增删改查视图组件</h2>

代码演示
---

```vue demo
<template>
  <UView>
    <USearch primary>
      <UInputItem field="name" label="茶叶名称"/>
      <UButton func='query' url='/doc/product/view'>查询</UButton>&nbsp;
      <AButton @click="add">新增</AButton>
    </USearch>
    <IvzPrimaryTable :bordered="true" :columns="columns" rowKey="id">
      <template #c_action="{record}">
        <UFuncTag func='edit' :data='record' url='/doc/product/edit'>编辑</UFuncTag>
        <ATag color="blue" @click="add">新增</ATag>
        <ATag color="red" @click="() => del(record)">删除</ATag>
      </template>
    </IvzPrimaryTable>
    <IvzPrimaryModal>
      <UInputItem field="name" label="茶叶名称"/>
      <template #title="{model}">
        {{model.id ? '编辑产品' : '新增产品'}}
      </template>
    </IvzPrimaryModal>
  </UView>
</template>

<script>
export default {
  name: "Demo",
  setup() {
    let columns = [
      {field: 'name', title: '产品名称'},
      {field: 'type', title: '产品类型'},
      {field: 'action', type:'action', title: '操作'},
    ]
    return {columns}
  },
  mounted() {
    this.$view.query();
  },
  methods: {
    add() {
      this.$view.openForAdd();
    },
    del(row) {
      this.$view.del('/doc/product/del', [row.id]);
    },
    query() {
      this.$view.query('/doc/product/view');
    }
  }
}
</script>
```
