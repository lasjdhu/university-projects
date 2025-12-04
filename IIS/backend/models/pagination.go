/**
 * IIS Project
 * @author Dias Tursynbayev
 */
package models

type PaginationAnswer[T any] struct {
	Data         []T `json:"data"`
	TotalRecords int `json:"total_records"`
	TotalPages   int `json:"total_pages"`
	CurrentPage  int `json:"page"`
	Limit        int `json:"limit"`
}
type Pagination struct {
	TotalRecords int `json:"total_records"`
	TotalPages   int `json:"total_pages"`
	CurrentPage  int `json:"page"`
	Limit        int `json:"limit"`
}
