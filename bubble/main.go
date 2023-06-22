package main

import (
	"fmt"
	"github.com/gin-gonic/gin"
	"github.com/jinzhu/gorm"
	_ "github.com/jinzhu/gorm/dialects/mysql"
	"net/http"
)

var (
	DB *gorm.DB
)

//数据库初始化
func initMySQL() (err error) {
	DB, err = gorm.Open("mysql", "root:123456@(43.136.243.4:3306)/bubble?charset=utf8mb4&parseTime=True&loc=Local")
	if err != nil {
		return
	}
	return DB.DB().Ping()
}

//Todo model
type Todo struct {
	ID     int64  `json:"id"`
	Title  string `json:"title"`
	Status bool   `json:"status"`
}

func main() {
	//创建数据库
	//sql:create database bubble
	//连接数据库
	err := initMySQL()
	if err != nil {
		panic(err)
	}
	//延迟关闭数据库
	defer DB.Close()
	//模型绑定关系
	DB.AutoMigrate(&Todo{})

	r := gin.Default()
	//告诉gin框架从模板文件引用的静态文件在哪里去找
	r.Static("/static", "static")
	//告诉gin框架从哪个目录加载文件
	r.LoadHTMLGlob("templates/*")
	r.GET("/", func(c *gin.Context) {
		c.HTML(http.StatusOK, "index.html", nil)
	})

	//v1
	v1Group := r.Group("v1")
	{
		//待办事项
		//添加
		v1Group.POST("/todo", func(c *gin.Context) {
			//前端页面提交待办事项 点击提交 发送请求到这里
			//从请求中把数据拿出来
			var todo Todo
			err := c.BindJSON(&todo)
			if err != nil {
				fmt.Printf("Get data failed, err: %v\n", err)
				return
			}
			//存入数据库
			err = DB.Create(&todo).Error
			//返回响应
			if err != nil {
				c.JSON(http.StatusOK, gin.H{"error": err.Error()})
			} else {
				c.JSON(http.StatusOK, todo)
			}

		})
		//查看所有待办事项
		v1Group.GET("/todo", func(c *gin.Context) {
			var todolist []Todo
			if err := DB.Find(&todolist).Error; err != nil {
				c.JSON(http.StatusOK, gin.H{"error": err.Error()})
			} else {
				c.JSON(http.StatusOK, todolist)
			}
		})
		//查看某个待办事项
		v1Group.GET("/todo/:id", func(c *gin.Context) {

		})
		//修改
		v1Group.PUT("/todo/:id", func(c *gin.Context) {
			id, ok := c.Params.Get("id")
			if !ok {
				c.JSON(http.StatusOK, gin.H{"error": "id不存在"})
			}
			var todo Todo
			if err = DB.Where("id=?", id).First(&todo).Error; err != nil {
				c.JSON(http.StatusOK, gin.H{"error": err.Error()})
				return
			}
			err = c.BindJSON(&todo)
			if err != nil {
				c.JSON(http.StatusOK, gin.H{
					"error": "无效的 variable",
				})
				return
			}
			err = DB.Save(&todo).Error
			if err != nil {
				c.JSON(http.StatusOK, gin.H{
					"error": err.Error(),
				})
				return
			} else {
				c.JSON(http.StatusOK, todo)
			}
		})
		//删除
		v1Group.DELETE("/todo/:id", func(c *gin.Context) {
			id, ok := c.Params.Get("id")
			if !ok {
				c.JSON(http.StatusOK, gin.H{
					"error": "无效的 id"})
				return
			}
			err := DB.Where("id=?", id).Delete(Todo{}).Error
			if err != nil {
				c.JSON(http.StatusOK, gin.H{
					"error": err.Error(),
				})
			} else {
				c.JSON(http.StatusOK, gin.H{
					id: "deleted",
				})
			}
		})
	}
	r.Run(":9000")
}
